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

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "main.h"
#include "common.h"
#include "regexpmatch.h"
#include "help.h"
#include "gui.h"

#define EXTS "gif|jpe?g|pdf|png|tiff?"
Opts *opts, *opts_v;
Tags *tags;

int main(int argc, char **argv) {
	opts = structopts(argc, argv);
	if(argc == 1) {
		help(0);
		return 0;
	}
	opts_v = (Opts*) malloc(sizeof(Opts));
	if(!opts_v) {
		fprintf(stderr, "malloc() failed: insufficient memory.\n");
		exit(EXIT_FAILURE);
	}
	memcpy(opts_v, opts, sizeof(Opts));
	for(int i = 0; i < opts_v->args_count; i++) {
		opts_v->args[i] = realpath(opts->args[i], NULL);
	}
	tags = (Tags*) malloc(sizeof(Tags));
	if(opts_v->g) {
		gui(opts);
	} else {
		if(!opts_v->args_count) {
			fprintf(stderr, "No paths to search in.\n");
			exit(EXIT_SUCCESS);
		}
		begin();
	}
	return 0;
}
void begin() {
/* Приведение строк тэгов в массивы */
	int count;
	if(opts_v->a) {
		tags->a = strsplit(opts_v->a, "\\s*,\\s*", 0, 8, &count);
		tags->a_count = count;
	}
	if(opts_v->o) {
		tags->o = strsplit(opts_v->o, "\\s*,\\s*", 0, 8, &count);
		tags->o_count = count;
	}
	if(opts_v->n) {
		tags->n = strsplit(opts_v->n, "\\s*,\\s*", 0, 8, &count);
		tags->n_count = count;
	}
	if(opts_v->i) {
		tags->i = strsplit(opts_v->i, "\\s*,\\s*", 0, 8, &count);
		tags->i_count = count;
	}
	if(opts_v->d) {
		tags->d = strsplit(opts_v->d, "\\s*,\\s*", 0, 8, &count);
		tags->d_count = count;
	}
	if(opts_v->c) {
		char **tags_c_tmp = (char**) malloc(sizeof(char*));
		tags_c_tmp = strsplit(opts_v->c, "\\s*,\\s*", 0, 8, &count);
		tags->c_count = count;
		if(tags->c_count % 2) {
			if(opts_v->g) {
				show_message(
					GTK_MESSAGE_ERROR, "Odd number of tags to replace."
				);
				return;
			} else {
				fprintf(
					stderr, "Odd number of elements in the value of key -c.\n"
				);
				exit(EXIT_FAILURE);
			}
		}
		tags->c = (char***) malloc(sizeof(char**));
		for(int i = 0; i < tags->c_count; i += 2) {
			tags->c[i / 2] = (char**) malloc(sizeof(char*));
			tags->c[i / 2][0] = (char*) malloc(strlen(tags_c_tmp[i]) + 1);
			tags->c[i / 2][1] = (char*) malloc(strlen(tags_c_tmp[i + 1]) + 1);
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
		free(tags_c_tmp);
	}
/* Генерация случайного имени каталога для сохранения ссылок */
	srand(time(NULL));
	int tosave = 1;
	if(!opts_v->s) {
		tosave = 0;
		opts_v->s = (char*) malloc(63);
		opts_v->s[62] = '\0';
		unsigned int rand_char;
		for(int i = 0; i < 16; i++) {
			do {
				rand_char = (unsigned int) rand() % 123;
			} while(
				rand_char < 48
				|| rand_char > 57 && rand_char < 65
				|| rand_char > 90 && rand_char < 97
			);
			opts_v->s[i] = (char) rand_char;
		}
	}
/* Создание списка путей ко всем анализируемым файлам */
	char *command = "";
	char **fileslist;
	int listlength;
	for(int i = 0; i < opts_v->args_count; i++) {
		command = strconcat(command, strconcat(" ", opts_v->args[i]));
	}
	command = strconcat(strconcat(strconcat(strconcat(
		"find",
		command),
		" -type f -regextype posix-extended -regex '.*\\.("),
		EXTS),
		")$'");
	fileslist = command_output_lines(command, &listlength);
	free(command);
	listlength = 0;
}
