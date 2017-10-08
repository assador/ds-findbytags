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
 * Dmitry Sokolov <dmitry@sokolov.website>
 **/

#include <stdio.h>
#include "help.h"

char *help_text =
	"\n"
	"ds-findbytags, v2.0.0, CLI / GTK+ 2\n"
	"Copyright © 2016-2017 Dmitry Sokolov\n"
	"\n"
	"This program is free software: you can redistribute it and/or modify\n"
	"it under the terms of the GNU General Public License as published by\n"
	"the Free Software Foundation, either version 3 of the License, or\n"
	"(at your option) any later version.\n"
	"\n"
	"This program is distributed in the hope that it will be useful,\n"
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	"GNU General Public License for more details.\n"
	"\n"
	"You should have received a copy of the GNU General Public License\n"
	"along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
	"\n"
	"Программа ищет в указанных каталогах изображения с ключевыми словами (тэгами),\n"
	"заданными в метаданных XMP или IPTC и удовлетворяющими заданным логическим\n"
	"условиям И, ИЛИ, НЕ. Опционально найденные изображения откроются в указанной\n"
	"программе. Попутно в найденных изображениях можно массово добавить, удалить\n"
	"или заменить тэги, сохранить ссылки на эти изображения в указанный каталог\n"
	"с оригинальными или случайными именами.\n"
	"Подробнее: http://sokolov.website/programs/ds-utils/ds-findbytags\n"
	"\n"
	"Зависимости: exiv2\n"
	"Рекомендуется: gtk2\n"
	"\n"
	"ds-findbytags [-g] [-k] [-t путь_к_файлу_тэгов] [-e кодировка]\n"
	"              [-a \"тэг, тэг, ...\"] [-o \"тэг, тэг, ...\"] [-n \"тэг, тэг, ...\"]\n"
	"              [-i \"тэг, тэг, ...\"] [-d \"тэг, тэг, ...\"]\n"
	"              [-c \"тэг, тэг, [тэг, тэг], ...\"] [-l просмотрщик]\n"
	"              [-s путь_к_сохраняемым_ссылкам] путь(-и)_где_искать\n"
	"\n"
	"-g  — от GUI       — При наличии ключа скрипт запускается в GTK-интерфейсе\n"
	"                     вместо CLI-интерфейса.\n"
	"-k  — от keep      — При наличии ключа ссылки именуются по именам найденных\n"
	"                     файлов, при отсутствии ключа ссылки именуются случайным\n"
	"                     набором символов.\n"
	"-t  — от tags      — Путь к XML-файлу с деревом тэгов в формате Geeqie\n"
	"                     (по умолчанию — $HOME/.config/geeqie/geeqierc.xml).\n"
	"                     Всё, не относящееся в этом файле к тэгам, игнорируется.\n"
	"                     Если неверен, дерево тэгов не показывается.\n"
	"                     Тэги из дерева добавляются перетаскиванием в нужное поле\n"
	"                     или, при двойным клике правой кнопкой мыши, в поле «И».\n"
	"-e  — от encoding  — Кодировка XML-файла тэгов в формате Geeqie\n"
	"                     (по умолчанию — * — автоопределение с помощью enca).\n"
	"-a  — от and       — При соответствии каждому из списка тэгов.\n"
	"-o  — от or        — При соответствии любому из списка тэгов.\n"
	"-n  — от not       — За исключением любого из списка тэгов.\n"
	"-i  — от insert    — Добавить тэги в найденные файлы.\n"
	"-d  — от delete    — Удалить тэги из найденных файлов.\n"
	"-c  — от change    — Заменить тэги в найденных файлах.\n"
	"-l  — от look      — Программа, в которой открывается каталог с найденными\n"
	"                     изображениями; по умолчанию — Geeqie. Если не нужно\n"
	"                     открывать его вообще, значение ключа должно быть no.\n"
	"-s  — от save      — Каталог (абсолютный путь), в котором сохраняются ссылки\n"
	"                     на найденное после окончания работы скрипта. Если ключ\n"
	"                     не указан, создаётся временный каталог по месту запуска\n"
	"                     скрипта, который после окончания его работы удаляется.\n"
	"\n"
	"Пример: ds-findbytags -g -k -t \"~/.config/geeqie/geeqierc.xml\"\n"
	"                      -a \"море, серые тучки, камни\" -o \"чайки, дельфины\" \\\n"
	"                      -n \"акулы, люди\" -i \"ах\" -d \"море\" -c \"чайки, птички\" \\\n"
	"                      -l \"no\" -s \"~/chosen\" ~/photos1 ~/photos2\n"
	"\n"
	"Dmitry Sokolov <dmitry@sokolov.website>\n"
;

void help(int gui) {
	if(!gui) {
		printf("%s\n", help_text);
	}
}
