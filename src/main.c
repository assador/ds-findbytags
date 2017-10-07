/**
 * ds-findbytags, v2.0.0, CLI / GTK+ 2
 * Copyright © 2016-2017 Dmitry Sokolov
 * 
 * This file is part of ds-findbytags.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Ищет в указанных каталогах изображения с ключевыми словами (тэгами),
 * заданными в метаданных XMP или IPTC и удовлетворяющими заданным логическим
 * условиям И, ИЛИ, НЕ. Опционально найденные изображения откроются в указанной
 * программе. Попутно в найденных изображениях можно массово добавить, удалить
 * или заменить тэги, сохранить ссылки на эти изображения в указанный каталог
 * с оригинальными или случайными именами.
 * Подробнее: http://sokolov.website/programs/ds-utils/ds-findbytags
 * 
 * Зависимости: exiv2
 * Рекомендуется: gtk2
 * 
 * ds-findbytags [-g] [-k] [-t путь_к_файлу_тэгов] [-e кодировка]
 *               [-a "тэг, тэг, ..."] [-o "тэг, тэг, ..."] [-n "тэг, тэг, ..."]
 *               [-i "тэг, тэг, ..."] [-d "тэг, тэг, ..."]
 *               [-c "тэг, тэг, [тэг, тэг], ..."] [-l просмотрщик]
 *               [-s путь_к_сохраняемым_ссылкам] путь(-и)_где_искать
 * 
 * -g  — от GUI       — При наличии ключа скрипт запускается в GTK-интерфейсе
 *                      вместо CLI-интерфейса.
 * -k  — от keep      — При наличии ключа ссылки именуются по именам найденных
 *                      файлов, при отсутствии ключа ссылки именуются случайным
 *                      набором символов.
 * -t  — от tags      — Путь к XML-файлу с деревом тэгов в формате Geeqie
 *                      (по умолчанию — $HOME/.config/geeqie/geeqierc.xml).
 *                      Всё, не относящееся в этом файле к тэгам, игнорируется.
 *                      Если неверен, дерево тэгов не показывается.
 *                      Тэги из дерева добавляются перетаскиванием в нужное поле
 *                      или, при двойным клике правой кнопкой мыши, в поле «И».
 * -e  — от encoding  — Кодировка XML-файла тэгов в формате Geeqie
 *                      (по умолчанию — * — автоопределение с помощью enca).
 * -a  — от and       — При соответствии каждому из списка тэгов.
 * -o  — от or        — При соответствии любому из списка тэгов.
 * -n  — от not       — За исключением любого из списка тэгов.
 * -i  — от insert    — Добавить тэги в найденные файлы.
 * -d  — от delete    — Удалить тэги из найденных файлов.
 * -c  — от change    — Заменить тэги в найденных файлах.
 * -l  — от look      — Программа, в которой открывается каталог с найденными
 *                      изображениями; по умолчанию — Geeqie. Если не нужно
 *                      открывать его вообще, значение ключа должно быть no.
 * -s  — от save      — Каталог (абсолютный путь), в котором сохраняются ссылки
 *                      на найденное после окончания работы скрипта. Если ключ
 *                      не указан, создаётся временный каталог по месту запуска
 *                      скрипта, который после окончания его работы удаляется.
 * 
 * Пример: ds-findbytags -g -k -t "~/.config/geeqie/geeqierc.xml"
 *                       -a "море, серые тучки, камни" -o "чайки, дельфины" \
 *                       -n "акулы, люди" -i "ах" -d "море" -c "чайки, птички" \
 *                       -l "no" -s "~/chosen" ~/photos1 ~/photos2
 * 
 * Dmitry Sokolov <dmitry@sokolov.website>
 **/

#include "main.h"

#define EXTS "gif|jpe?g|pdf|png|tiff?"
const char *file_separator =
	#ifdef _WIN32
		"\\";
	#else
		"/";
	#endif
Opts *opts, *opts_v;
Tags *tags;

int main(int argc, char **argv) {
	setlocale(LC_ALL, "");
	bindtextdomain("ds-findbytags", "/usr/share/locale/");
	textdomain("ds-findbytags");
	srand(time(NULL));
	opts = structopts(argc, argv);
	if(argc == 1) {
		help(0);
		return 0;
	}
	opts_v = (Opts*) malloc(sizeof(Opts));
	if(!opts_v) {
		fprintf(stderr, _("main(): malloc() failed: insufficient memory.\n"));
		exit(EXIT_FAILURE);
	}
	memcpy(opts_v, opts, sizeof(Opts));
	for(int i = 0; i < opts_v->args_count; i++) {
		opts_v->args[i] = realpath(opts->args[i], NULL);
	}
	tags = (Tags*) malloc(sizeof(Tags));
	if(!tags) {
		fprintf(stderr, _("main(): malloc() failed: insufficient memory.\n"));
		exit(EXIT_FAILURE);
	}
	if(opts_v->g) {
		gui();
	} else {
		if(!opts_v->args_count) {
			fprintf(stderr, _("main(): No paths to search in.\n"));
			exit(EXIT_SUCCESS);
		}
		begin();
	}
	return 0;
}
int begin() {
/* Приведение строк тэгов в массивы */
	int count;
	if(opts_v->a) {
		tags->a = strsplit(opts_v->a, "\\s*,\\s*", 0, 0, 8, &count);
		tags->a_count = count;
	}
	if(opts_v->o) {
		tags->o = strsplit(opts_v->o, "\\s*,\\s*", 0, 0, 8, &count);
		tags->o_count = count;
	}
	if(opts_v->n) {
		tags->n = strsplit(opts_v->n, "\\s*,\\s*", 0, 0, 8, &count);
		tags->n_count = count;
	}
	if(opts_v->i) {
		tags->i = strsplit(opts_v->i, "\\s*,\\s*", 0, 0, 8, &count);
		tags->i_count = count;
	}
	if(opts_v->d) {
		tags->d = strsplit(opts_v->d, "\\s*,\\s*", 0, 0, 8, &count);
		tags->d_count = count;
	}
	if(opts_v->c) {
		char **tags_c_tmp = (char**) malloc(sizeof(char*));
		if(!tags_c_tmp) {
			fprintf(
				stderr,
				_("begin(): malloc() failed: insufficient memory.\n")
			);
			return 0;
		}
		tags_c_tmp = strsplit(opts_v->c, "\\s*,\\s*", 0, 0, 8, &count);
		if(count % 2) {
			if(opts_v->g) {
				show_message(
					GTK_MESSAGE_ERROR, _("Odd number of tags to replace.")
				);
				return 0;
			} else {
				fprintf(
					stderr,
					_(
						"begin(): Odd number of elements "
						"in the value of key -c.\n"
					)
				);
				exit(EXIT_FAILURE);
			}
		}
		tags->c_count = count / 2;
		tags->c = (char***) malloc(sizeof(char**));
		if(!tags->c) {
			fprintf(
				stderr,
				_("begin(): malloc() failed: insufficient memory.\n")
			);
			return 0;
		}
		for(int i = 0; i < tags->c_count; i += 2) {
			tags->c[i / 2] = (char**) malloc(sizeof(char*));
			if(!tags->c[i / 2]) {
				fprintf(
					stderr,
					_("begin(): malloc() failed: insufficient memory.\n")
				);
				return 0;
			}
			tags->c[i / 2][0] = (char*) malloc(strlen(tags_c_tmp[i]) + 1);
			if(!tags->c[i / 2][0]) {
				fprintf(
					stderr,
					_("begin(): malloc() failed: insufficient memory.\n")
				);
				return 0;
			}
			tags->c[i / 2][1] = (char*) malloc(strlen(tags_c_tmp[i + 1]) + 1);
			if(!tags->c[i / 2][1]) {
				fprintf(
					stderr,
					_("begin(): malloc() failed: insufficient memory.\n")
				);
				return 0;
			}
			*tags->c[i / 2][0] = '\0';
			*tags->c[i / 2][1] = '\0';
			strncat(
				tags->c[i / 2][0],
				tags_c_tmp[i],
				strlen(tags_c_tmp[i])
			);
			strncat(
				tags->c[i / 2][1],
				tags_c_tmp[i + 1],
				strlen(tags_c_tmp[i + 1])
			);
		}
		for(int i = 0; i < count; i++) free(tags_c_tmp[i]);
		free(tags_c_tmp);
	}
/* Генерация случайного имени каталога для сохранения ссылок */
	int tosave = 1;
	if(!opts_v->s) {
		tosave = 0;
		opts_v->s = random_name(16);
	}
/* Создание списка путей ко всем анализируемым файлам */
	char *command = "";
	for(int i = 0; i < opts_v->args_count; i++) {
		command = strconcat(command, " ");
		command = strconcat(command, opts_v->args[i]);
	}
	command = strconcat("find", command);
	command = strconcat(command,
		" -type f -regextype posix-extended -regex '.*\\.(");
	command = strconcat(command, EXTS);
	command = strconcat(command, ")$'");
/* Анализ каждого файла */
	unsigned int files_count = 0, reoffset = 0;
	File **files = (File**) malloc(sizeof(File*));
	if(!files) {
		fprintf(stderr, _("begin(): malloc() failed: insufficient memory.\n"));
		return 0;
	}
	char **filetags;
	char *exiv_com, *exiv_out, *filename_qtd;
	pcre *re_s = regexpcompile("^subject\\s*(?=\\S)(.+)$", PCRE_MULTILINE);
	pcre *re_k = regexpcompile("^Keywords\\s*(?=\\S)(.+)$", PCRE_MULTILINE);
	Reresult *reresult;
	size_t filename_size_step = 256, filename_size = filename_size_step;
	char *filename = (char*) malloc(filename_size);
	if(!filename) {
		fprintf(stderr, _("begin(): malloc() failed: insufficient memory.\n"));
		return 0;
	}
	FILE *found = popen(command, "r");
	if(!found) {
		fprintf(stderr, _("begin(): Cannot into popen.\n"));
		return 0;
	}
	/* Для каждого анализируемого файла */
	while(getline(&filename, &filename_size, found) != EOF) {
		if(strlen(filename) > filename_size - 1) {
			filename =
				(char*) realloc(filename, filename_size += filename_size_step);
			if(!filename) {
				fprintf(
					stderr,
					_("begin(): realloc() failed: insufficient memory.\n")
				);
				return 0;
			}
		}
		filename[strlen(filename) - 1] = '\0';
		filename_qtd = strconcat("'", filename);
		filename_qtd = strconcat(filename_qtd, "'");
		filetags = (char**) malloc(sizeof(char*));
		if(!filetags) {
			fprintf(
				stderr,
				_("begin(): malloc() failed: insufficient memory.\n")
			);
			return 0;
		}
		/* Сбор тэгов в XMP-метаданных */
		exiv_com = strconcat("exiv2 -PXnt ", filename_qtd);
		exiv_out = command_output(exiv_com);
		if(exiv_out) {
			reresult = regexpmatch_compiled(
				exiv_out, re_s, 0, sizeof(reresult->indexes)
			);
		}
		if(exiv_out && reresult->count > 1) {
			char tmp[reresult->indexes[3] - reresult->indexes[2]];
			tmp[0] = '\0';
			strncat(
				tmp,
				exiv_out + reresult->indexes[2],
				reresult->indexes[3] - reresult->indexes[2]
			);
			filetags = strsplit(tmp, "\\s*,\\s*", 0, 0, 8, &count);
		} else {
			/* Сбор тэгов в IPTC-метаданных, если не оказалось в XMP */
			exiv_com = strconcat("exiv2 -PInt ", filename_qtd);
			exiv_out = command_output(exiv_com);
			if(exiv_out) {
				reoffset = 0;
				count = 0;
				for(int i = 0; ; i++) {
					reresult = regexpmatch_compiled(
						exiv_out + reoffset, re_k, 0, sizeof(reresult->indexes)
					);
					if(reresult->count < 2) break;
					filetags = (char**) realloc(filetags, sizeof(char*) * (i + 1));
					if(!filetags) {
						fprintf(
							stderr,
							_("begin(): realloc() failed: "
							"insufficient memory.\n")
						);
						return 0;
					}
					filetags[i] = (char*) malloc(
						reresult->indexes[3] - reresult->indexes[2] + 1
					);
					if(!filetags[i]) {
						fprintf(
							stderr,
							_("begin(): malloc() failed: "
							"insufficient memory.\n")
						);
						return 0;
					}
					filetags[i][0] = '\0';
					strncat(
						filetags[i],
						exiv_out + reoffset + reresult->indexes[2],
						reresult->indexes[3] - reresult->indexes[2]
					);
					reoffset += reresult->indexes[1];
					count++;
				}
			}
		}
		if(exiv_out) {
			free(reresult->indexes);
			free(reresult);
			free(exiv_out);
			/* Проверка соответствия тэгов файла тэгам поиска */
			if(suitable(
				filetags, count, tags->a, tags->a_count,
				tags->o, tags->o_count, tags->n, tags->n_count
			)) {
				/* Действия над тэгами файла */
				if(actions(
					&filetags, &count, tags->i, tags->i_count,
					tags->d, tags->d_count, tags->c, tags->c_count
				) > 0) {
					char *tmp = strconcat(
						"exiv2 -M\"add Xmp.dc.subject\" "
							"-M\"del Iptc.Application2.Keywords\" ",
						filename_qtd
					);
					system(tmp);
					for(int i = 0; i < count; i++) {
						tmp = strconcat(
							"exiv2 -M\"set Xmp.dc.subject XmpBag ",
							filetags[i]
						);
						tmp = strconcat(
							tmp,
							"\" -M\"add Iptc.Application2.Keywords String "
						);
						tmp = strconcat(tmp, filetags[i]);
						tmp = strconcat(tmp, "\" ");
						tmp = strconcat(tmp, filename_qtd);
						system(tmp);
					}
					free(tmp);
				}
				/* Создание массива отобранных файлов */
				files_count++;
				files = (File**) realloc(files, sizeof(File*) * files_count);
				if(!files) {
					fprintf(
						stderr,
						_("begin(): realloc() failed: insufficient memory.\n")
					);
					return 0;
				}
				files[files_count - 1] = (File*) malloc(sizeof(File));
				if(!files[files_count - 1]) {
					fprintf(
						stderr,
						_("begin(): malloc() failed: insufficient memory.\n")
					);
					return 0;
				}
				files[files_count - 1]->fullpath = strdup(filename);
				files[files_count - 1]->fullname = strdup(filename);
				files[files_count - 1]->path =
					dirname(files[files_count - 1]->fullpath);
				files[files_count - 1]->name =
					basename(files[files_count - 1]->fullname);
			}
			for(int i = 0; i < count; i++) free(filetags[i]);
		}
		free(filetags);
		free(exiv_com);
	}
	if(pclose(found) != 0) {
		fprintf(stderr, _("begin(): Cannot into pclose.\n"));
		return 0;
	}
	pcre_free((void*) re_s);
	pcre_free((void*) re_k);
	free(command);
	free(filename); filename = NULL;
	free(filename_qtd); filename_qtd = NULL;
	/* Создание каталога, если его ещё нет, с указанным или случайным именем */
	struct stat stat_buffer;
	stat(opts_v->s, &stat_buffer);
	if(!S_ISDIR(stat_buffer.st_mode) && mkdir(opts_v->s, 0755) == -1) {
		fprintf(stderr, _("begin(): Cannot create %s.\n"), opts_v->s);
	}
	/* Создание символических ссылок отобранных файлов */
	for(int i = 0; i < files_count; i++) {
		if(opts_v->k) {
			filename = strdup(files[i]->name);
		} else {
			char *filename_base = random_name(16);
			Reresult *reresult = regexpmatch(
				files[i]->name, "\\..*?$", 0, 0, sizeof(reresult->indexes)
			);
			if(reresult->count != -1) {
				filename = (char*) malloc(
					17 + reresult->indexes[1] - reresult->indexes[0]
				);
				if(!filename) {
					fprintf(
						stderr,
						_("begin(): malloc() failed: insufficient memory.\n")
					);
					return 0;
				}
				filename[0] = '\0';
				strncat(filename, filename_base, 16);
				strncat(
					filename,
					files[i]->name + reresult->indexes[0],
					reresult->indexes[1] - reresult->indexes[0]
				);
				free(reresult->indexes);
			} else {
				filename = strdup(filename_base);
			}
			free(reresult);
			free(filename_base);
		}
		char *fullname = strdup(opts_v->s);
		fullname = (char*) realloc(
			fullname,
			strlen(fullname) + strlen(file_separator) + strlen(filename) + 1
		);
		if(!fullname) {
			fprintf(
				stderr,
				_("begin(): realloc() failed: insufficient memory.\n")
			);
			return 0;
		}
		strncat(fullname, file_separator, strlen(file_separator));
		strncat(fullname, filename, strlen(filename));
		symlink(files[i]->fullname, fullname);
		free(fullname);
	}
	free(filename); filename = NULL;
	/* Запуск указанной программы в каталоге с символическими ссылками */
	if(opts_v->l && strcmp(opts_v->l, "no") != 0) {
		char *tmp = strconcat(opts_v->l, " ");
		tmp = strconcat(tmp, opts_v->s);
		system(tmp);
		free(tmp);
	}
	/* Удаление временного каталога с символическими ссылками */
	if(!tosave) {
		char *tmp = strconcat("rm -rf ", opts_v->s);
		system(tmp);
		free(tmp);
	}
	/* Очистка */
	for(int i = 0; i < files_count; i++) {
		free(files[i]->fullpath);
		free(files[i]->fullname);
		free(files[i]);
	}
	free(files);
	for(int i = 0; i < tags->a_count; i++) free(tags->a[i]);
	for(int i = 0; i < tags->o_count; i++) free(tags->o[i]);
	for(int i = 0; i < tags->n_count; i++) free(tags->n[i]);
	for(int i = 0; i < tags->i_count; i++) free(tags->i[i]);
	for(int i = 0; i < tags->d_count; i++) free(tags->d[i]);
	for(int i = 0; i < tags->c_count; i++) {
		for(int y = 0; y < 2; y++) free(tags->c[i][y]);
		free(tags->c[i]);
	}
	free(tags->a); free(tags->o); free(tags->n);
	free(tags->i); free(tags->d); free(tags->c);
	tags->a = tags->o = tags->n = tags->i = tags->d = NULL; tags->c = NULL;
	tags->a_count = tags->o_count = tags->n_count =
	tags->i_count = tags->d_count = tags->c_count = 0;
	return 1;
}
/* Проверка соответствия тэгов файла тэгам поиска */
int suitable(
	char **t, int tc, char **a, int ac, char **o, int oc, char **n, int nc
) {
	int s = 1;
	/* Проверка соответствия тэгам И */
	for(int i = 0; i < ac; i++) {
		s = 0;
		for(int y = 0; y < tc; y++) {
			if(strcmp(a[i], t[y]) == 0) {s = 1; break;}
		}
		if(!s) return(0);
	}
	/* Проверка соответствия тэгам ИЛИ */
	for(int i = 0; i < oc; i++) {
		s = 0;
		for(int y = 0; y < tc; y++) {
			if(strcmp(o[i], t[y]) == 0) {s = 1; break;}
		}
		if(s) break;
	}
	if(!s) return(0);
	/* Проверка соответствия тэгам НЕ */
	for(int i = 0; i < nc; i++) {
		for(int y = 0; y < tc; y++) {
			if(strcmp(n[i], t[y]) == 0) return(0);
		}
	}
	return s;
}
/* Действия над тэгами файла */
int actions(
	char ***t, int *tc,
	char **in, int inc, char **de, int dec, char ***ch, int chc
) {
	int changed = 0, exists = 0;
	/* Добавление новых тэгов */
	for(int i = 0; i < inc; i++) {
		for(int y = 0; y < *tc; y++) {
			if(strcmp((*t)[y], in[i]) == 0) {exists = 1; break;}
		}
		if(!exists) {
			*t = (char**) realloc(*t, (++*tc) * sizeof(char*));
			if(!t) {
				fprintf(
					stderr,
					_("actions(): realloc() failed: insufficient memory.\n")
				);
				return 0;
			}
			(*t)[*tc - 1] = (char*) malloc(strlen(in[i]) + 1);
			if(!(*t)[*tc - 1]) {
				fprintf(
					stderr,
					_("actions(): malloc() failed: insufficient memory.\n")
				);
				return 0;
			}
			(*t)[*tc - 1][strlen(in[i])] = '\0';
			strncpy((*t)[*tc - 1], in[i], strlen(in[i]));
			changed++;
		}
		exists = 0;
	}
	/* Удаление существующих тэгов */
	for(int i = 0; i < dec; i++) {
		for(int y = 0; y < *tc; y++) {
			if(strcmp((*t)[y], de[i]) == 0) {
				free((*t)[y]);
				for(int z = y; z < *tc - 1; z++) (*t)[z] = (*t)[z + 1];
				*t = (char**) realloc(*t, (--*tc) * sizeof(char*));
				if(!t) {
					fprintf(
						stderr,
						_("actions(): realloc() failed: insufficient memory.\n")
					);
					return 0;
				}
				changed++;
				y--;
			}
		}
	}
	/* Замена существующих тэгов */
	for(int i = 0; i < chc; i++) {
		for(int y = 0; y < *tc; y++) {
			if(strcmp((*t)[y], ch[i][0]) == 0) {
				(*t)[y] = (char*) realloc((*t)[y], strlen(ch[i][1]) + 1);
				if(!(*t)[y]) {
					fprintf(
						stderr,
						_("actions(): realloc() failed: insufficient memory.\n")
					);
					return 0;
				}
				(*t)[y][strlen(ch[i][1])] = '\0';
				strncpy((*t)[y], ch[i][1], strlen(ch[i][1]));
				changed++;
			}
		}
	}
	return changed;
}