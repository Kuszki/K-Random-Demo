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

	ui->Progress->hide();

	ui->Samples->setMaximum(INT_MAX);

	ui->Seed->setMaximum(INT_MAX);
	ui->Seed->setValue(time(nullptr));

	ui->Plot->yAxis->setRange(0.0, 100.0);
	ui->Plot->xAxis->setRange(-0.5, 10.5);

	ui->Plot->axisRect()->setRangeDrag(Qt::Vertical);
	ui->Plot->axisRect()->setRangeZoom(Qt::Vertical);

	ui->Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	Bars = new QCPBars(ui->Plot->xAxis, ui->Plot->yAxis);
	About = new AboutDialog(this);
	Thread = new QThread(this);
	Worker = new RandomWorker;

	stopLocked << ui->actionStop << ui->actionPause;

	runLocked << ui->actionRun << ui->runButton  << ui->refreshTool << ui->actionClear << ui->Autoclear << ui->Distribution << ui->Engine
			<< ui->Seed << ui->Samples << ui->Max << ui->Min << ui->Medium << ui->Lambda << ui->Rmean << ui->Imean << ui->Sigma << ui->Speed;

	connect(ui->Plot->yAxis, SIGNAL(rangeChanged(const QCPRange&, const QCPRange&)), SLOT(PlotRangeChanged(const QCPRange&, const QCPRange&)));

	connect(Worker, &RandomWorker::onProgressBegin, this, boost::bind(&MainWindow::SwitchUiStatus, this, Locked));
	connect(Worker, &RandomWorker::onProgressResume, this, boost::bind(&MainWindow::SwitchUiStatus, this, Locked));

	connect(Worker, &RandomWorker::onResultsReady, this, &MainWindow::PlotReadyResult, Qt::QueuedConnection);
	connect(Worker, &RandomWorker::onProgressEnd, this, &MainWindow::StopActionClicked, Qt::QueuedConnection);

	connect(ui->actionRun, &QAction::triggered, Worker, &RandomWorker::resumeProgress, Qt::DirectConnection);
	connect(ui->actionPause, &QAction::triggered, Worker, &RandomWorker::pauseProgress, Qt::DirectConnection);
	connect(ui->actionStop, &QAction::triggered, Worker, &RandomWorker::stopProgress, Qt::DirectConnection);

	connect(Worker, &RandomWorker::onProgressBegin, ui->Progress, &QProgressBar::show, Qt::QueuedConnection);
	connect(Worker, &RandomWorker::onProgressBegin, ui->Progress, &QProgressBar::setRange, Qt::QueuedConnection);
	connect(Worker, &RandomWorker::onProgressEnd, ui->Progress, &QProgressBar::hide, Qt::QueuedConnection);
	connect(Worker, &RandomWorker::onProgressUpdate, ui->Progress, &QProgressBar::setValue, Qt::QueuedConnection);

	connect(ui->actionAbout, &QAction::triggered, About, &AboutDialog::show);

	Worker->moveToThread(Thread);
	Thread->start();

	DistributionValueChanged(0);
	SwitchUiStatus(Unlocked);
}

MainWindow::~MainWindow(void)
{
	Worker->stopProgress();

	Thread->exit();
	Thread->wait();

	delete Worker;
	delete ui;
}

void MainWindow::SwitchUiStatus(MainWindow::UiStatus Status)
{
	switch (Status)
	{
		case Locked:
		case Unlocked:
		{
			bool Lock = Status == Locked;

			for (auto Control : runLocked)
			{
				if (auto A = qobject_cast<QAction*>(Control)) A->setEnabled(!Lock);
				else if (auto W = qobject_cast<QWidget*>(Control)) W->setEnabled(!Lock);
			}

			for (auto Control : stopLocked)
			{
				if (auto A = qobject_cast<QAction*>(Control)) A->setEnabled(Lock);
				else if (auto W = qobject_cast<QWidget*>(Control)) W->setEnabled(Lock);
			}
		}
		break;
	}
}

void MainWindow::PlotRangeChanged(const QCPRange& New, const QCPRange& Old)
{
	if (New.lower < 0) ui->Plot->yAxis->setRange(0, Old.size());
}

void MainWindow::PlotReadyResult(const QMap<int, int>& Samples)
{
	for (auto i = Samples.begin(); i != Samples.end(); ++i)
	{
		Results[i.key()] += i.value();
	}

	if (!thread()->eventDispatcher()->hasPendingEvents())
	{
		auto Values = Results.values().toVector();
		double Sum = 0.0;

		for (const auto& I : Values) Sum += I;
		for (auto& I : Values) I = 100 * (I / Sum);

		Bars->setData(Results.keys().toVector(), Values);

		ui->Plot->replot();
	}
}

void MainWindow::DistributionValueChanged(int Distribution)
{
	ui->minLabel->hide(); ui->Min->hide();
	ui->maxLabel->hide(); ui->Max->hide();
	ui->mediumLabel->hide(); ui->Medium->hide();
	ui->lambdaLabel->hide(); ui->Lambda->hide();
	ui->rmeanLabel->hide(); ui->Rmean->hide();
	ui->imeanLabel->hide(); ui->Imean->hide();
	ui->sigmaLabel->hide(); ui->Sigma->hide();

	switch (Distribution)
	{
		case 0:
			ui->minLabel->show(); ui->Min->show();
			ui->maxLabel->show(); ui->Max->show();
		break;
		case 1:
			ui->lambdaLabel->show(); ui->Lambda->show();
		break;
		case 2:
			ui->imeanLabel->show(); ui->Imean->show();
		break;
		case 3:
			ui->rmeanLabel->show(); ui->Rmean->show();
			ui->sigmaLabel->show(); ui->Sigma->show();
		break;
		case 4:
			ui->minLabel->show(); ui->Min->show();
			ui->maxLabel->show(); ui->Max->show();
			ui->mediumLabel->show(); ui->Medium->show();
		break;
	}
}

void MainWindow::RangeSpinChanged(void)
{
	ui->Max->setMinimum(ui->Min->value() + 1);
	ui->Min->setMaximum(ui->Max->value() - 1);
	ui->Medium->setMinimum(ui->Min->value());
	ui->Medium->setMaximum(ui->Max->value());
}

void MainWindow::RefreshButtonClicked(void)
{
	ui->Seed->setValue(time(nullptr));
}

void MainWindow::RunActionClicked(void)
{
	if (Worker->getStatus() == RandomWorker::Stopped)
	{
		const int Samples = ui->Samples->value();
		const int Speed = ui->Speed->value();

		const int Seed = ui->Seed->value();
		const int Engine = ui->Engine->currentIndex();
		const int Distribution = ui->Distribution->currentIndex();

		unsigned Sleep(Speed ? 1000 / Speed : 0);
		unsigned Block(1), Base(1000);
		double P1(0.0), P2(0.0), P3(0.0);

		switch (Distribution)
		{
			case 0:
				P1 = ui->Min->value();
				P2 = ui->Max->value();
			break;
			case 1:
				P1 = ui->Lambda->value();
			break;
			case 2:
				P1 = ui->Imean->value();
			break;
			case 3:
				P1 = ui->Rmean->value();
				P2 = ui->Sigma->value();
			break;
			case 4:
				P1 = ui->Min->value();
				P2 = ui->Medium->value();
				P3 = ui->Max->value();
			break;
		}

		if (Speed) while (!Sleep)
		{
			Base *= 10;
			Block *= 10;

			Sleep = Base / Speed;
		}
		else
		{
			Block = 5000;
			Sleep = 0;
		}

		if (ui->Autoclear->isChecked()) ClearActionClicked();

		Worker->setGenerator(P1, P2, P3, Seed, Distribution, Engine);
		Worker->setSamples(Samples);
		Worker->setBlocks(Block);
		Worker->setSleep(Sleep);

		Worker->startProgress();
	}
}

void MainWindow::StopActionClicked(void)
{
	SwitchUiStatus(Unlocked);
	AdjustActionClicked();
}

void MainWindow::PauseActionClicked(void)
{
	ui->actionRun->setEnabled(true);
	ui->actionPause->setEnabled(false);
}

void MainWindow::AdjustActionClicked(void)
{
	Bars->rescaleAxes();

	ui->Plot->replot();
}

void MainWindow::ClearActionClicked(void)
{
	Bars->clearData();
	Results.clear();

	ui->Plot->replot();
}
