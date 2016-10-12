/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Boost.Random demo with gui writen in Qt5                               *
 *  Copyright (C) 2016  Łukasz "Kuszki" Dróżdż  l.drozdz@openmailbox.org   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the  Free Software Foundation, either  version 3 of the  License, or   *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This  program  is  distributed  in the hope  that it will be useful,   *
 *  but WITHOUT ANY  WARRANTY;  without  even  the  implied  warranty of   *
 *  MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the   *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have  received a copy  of the  GNU General Public License   *
 *  along with this program. If not, see http://www.gnu.org/licenses/.     *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *Parent)
: QMainWindow(Parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->Seed->setMaximum(INT_MAX);
	ui->Seed->setValue(time(nullptr));

	ui->Plot->yAxis->setRange(0.0, 100.0);
	ui->Plot->xAxis->setRange(-0.5, 10.5);

	ui->Plot->axisRect()->setRangeDrag(Qt::Vertical);
	ui->Plot->axisRect()->setRangeZoom(Qt::Vertical);

	ui->Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	Bars = new QCPBars(ui->Plot->xAxis, ui->Plot->yAxis);

	connect(ui->Plot->yAxis, SIGNAL(rangeChanged(const QCPRange&, const QCPRange&)), SLOT(PlotRangeChanged(const QCPRange&, const QCPRange&)));
}

MainWindow::~MainWindow(void)
{
	delete ui;
}

void MainWindow::PlotRangeChanged(const QCPRange& New, const QCPRange& Old)
{
	if (New.lower < 0) ui->Plot->yAxis->setRange(0, Old.size());
}


void MainWindow::AdjustActionClicked(void)
{
	Bars->rescaleAxes();

	ui->Plot->replot();
}
