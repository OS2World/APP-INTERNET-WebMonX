#!/bin/sh

#
# WebMonX
#  http://debfx.fobos.de/webmonx/
#
# Copyright (C) 2006-2010 Felix Geyer <debfx-webmonx@fobos.de>
#
# This file is part of WebMonX.
#
# WebMonX is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 2 or (at your option) version 3 as published by
# the Free Software Foundation.
#
# WebMonX is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>
# or write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

rm -f ../share/icons/16x16/*.png
rm -f ../share/icons/24x24/*.png
rm -f ../share/icons/32x32/*.png
rm -f ../share/icons/pixmaps/*.xpm
rm -f ../ico/*.ico

convert package_network.png -draw 'image Over 14,14 16,16 viewmag16.png' -resize 16x16 -quality 90 ../share/icons/16x16/webmonx.png
convert package_network.png -draw 'image Over 14,14 18,18 knewstuff.png' -resize 16x16 -quality 90 ../share/icons/16x16/webmonx_update.png
convert package_network.png -draw 'image Over 14,14 16,16 viewmag16.png' -resize 32x32 -quality 90 ../share/icons/32x32/webmonx.png
convert package_network.png -draw 'image Over 14,14 16,16 viewmag16.png' -resize 32x32 ../share/icons/pixmaps/webmonx.xpm

convert filenew2.png -resize 24x24 -quality 90 ../share/icons/24x24/entry_add.png
convert edit.png -resize 24x24 -quality 90 ../share/icons/24x24/entry_modify.png
convert entry_delete.png -resize 24x24 -quality 90 ../share/icons/24x24/entry_delete.png
convert entry_toggle.png -resize 24x24 -quality 90 ../share/icons/24x24/entry_toggle.png
convert viewmag32.png -resize 24x24 -quality 90 ../share/icons/24x24/check_selected.png
convert viewmag32.png -resize 24x24 -draw 'image Over 1,15 9,9 add.png' -quality 90 ../share/icons/24x24/check_all.png
convert viewmag32.png -draw 'image Over 2,19 11,11 button_cancel.png' -resize 24x24 -quality 90 ../share/icons/24x24/check_cancel.png

cp configure.png ../share/icons/16x16/configure.png
cp exit.png ../share/icons/16x16/quit.png
convert filenew1.png -resize 16x16 -quality 90 ../share/icons/16x16/entry.png
convert filenew1.png -draw 'image Over 15,15 16,16 cancel2.png' -resize 16x16 -quality 90 ../share/icons/16x16/entry_disabled.png
convert entry_error.png -resize 16x16 -quality 90 ../share/icons/16x16/entry_error.png
convert entry_error.png -draw 'image Over 15,15 16,16 cancel2.png' -resize 16x16 -quality 90 ../share/icons/16x16/entry_error_disabled.png
convert entry_updated.png -resize 16x16 -quality 90 ../share/icons/16x16/entry_updated.png
convert entry_updated.png -draw 'image Over 15,15 16,16 cancel2.png' -resize 16x16 -quality 90 ../share/icons/16x16/entry_updated_disabled.png
convert filenew1.png -draw 'image Over 7,8 16,16 viewmag16.png' -resize 16x16 -quality 90 ../share/icons/16x16/entry_checking.png

icotool -c -o ../ico/webmonx.ico ../share/icons/16x16/webmonx.png ../share/icons/32x32/webmonx.png
