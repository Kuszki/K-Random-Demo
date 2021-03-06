#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#*                                                                         *
#*  Boost.Random demo with gui writen in Qt5                               *
#*  Copyright (C) 2016  Łukasz "Kuszki" Dróżdż  l.drozdz@openmailbox.org   *
#*                                                                         *
#*  This program is free software: you can redistribute it and/or modify   *
#*  it under the terms of the GNU General Public License as published by   *
#*  the  Free Software Foundation, either  version 3 of the  License, or   *
#*  (at your option) any later version.                                    *
#*                                                                         *
#*  This  program  is  distributed  in the hope  that it will be useful,   *
#*  but WITHOUT ANY  WARRANTY;  without  even  the  implied  warranty of   *
#*  MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the   *
#*  GNU General Public License for more details.                           *
#*                                                                         *
#*  You should have  received a copy  of the  GNU General Public License   *
#*  along with this program. If not, see http://www.gnu.org/licenses/.     *
#*                                                                         *
#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

QT			+=	core gui widgets printsupport concurrent

TARGET		=	K-Random-Demo
TEMPLATE		=	app
CONFIG		+=	c++14

SOURCES		+=	main.cpp \
				mainwindow.cpp \
				randomworker.cpp \
				aboutdialog.cpp

HEADERS		+=	mainwindow.hpp \
				randomworker.hpp \
				aboutdialog.hpp

FORMS		+=	mainwindow.ui \
				aboutdialog.ui

RESOURCES		+=	resources.qrc

LIBS			+=	-lqcustomplot

TRANSLATIONS	+=	k-random-demo_pl.ts

QMAKE_CXXFLAGS	+=	-s -march=native -std=c++14
