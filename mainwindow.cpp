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

	ui->Samples->setMaximum(INT_MAX);

	ui->Seed->setMaximum(INT_MAX);
	ui->Seed->setValue(time(nullptr));

	ui->Threads->setValue(QThreadPool::globalInstance()->maxThreadCount());

	ui->Plot->yAxis->setRange(0.0, 100.0);
	ui->Plot->xAxis->setRange(-0.5, 10.5);

	ui->Plot->axisRect()->setRangeDrag(Qt::Vertical);
	ui->Plot->axisRect()->setRangeZoom(Qt::Vertical);

	ui->Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	Bars = new QCPBars(ui->Plot->xAxis, ui->Plot->yAxis);
	Progress = new QProgressBar(this);

	ui->statusBar->addPermanentWidget(Progress);

	stopLocked << ui->actionStop << ui->actionPause;

	runLocked << ui->actionRun << ui->runButton << ui->Distribution << ui->Engine
			<< ui->Seed << ui->Samples << ui->Max << ui->Min << ui->Threads;

	connect(ui->Plot->yAxis, SIGNAL(rangeChanged(const QCPRange&, const QCPRange&)), SLOT(PlotRangeChanged(const QCPRange&, const QCPRange&)));

	connect(&Watcher, &QFutureWatcher<QMap<int, double>>::finished, boost::bind(&MainWindow::SwitchUiStatus, this, Unlocked));
	connect(&Watcher, &QFutureWatcher<QMap<int, double>>::finished, this, &MainWindow::AdjustActionClicked, Qt::QueuedConnection);
	connect(&Watcher, &QFutureWatcher<QMap<int, double>>::resultReadyAt, this, &MainWindow::PlotReadyResult, Qt::QueuedConnection);

	connect(&Watcher, &QFutureWatcher<QMap<int, double>>::progressRangeChanged, Progress, &QProgressBar::setRange);
	connect(&Watcher, &QFutureWatcher<QMap<int, double>>::progressValueChanged, Progress, &QProgressBar::setValue);
	connect(&Watcher, &QFutureWatcher<QMap<int, double>>::started, Progress, &QProgressBar::show);
	connect(&Watcher, &QFutureWatcher<QMap<int, double>>::finished, Progress, &QProgressBar::hide);

	connect(ui->actionPause, &QAction::triggered, &Watcher, &QFutureWatcher<QMap<int, double>>::pause);
	connect(ui->actionStop, &QAction::triggered, &Watcher, &QFutureWatcher<QMap<int, double>>::cancel);

	Progress->hide();

	SwitchUiStatus(Unlocked);
}

MainWindow::~MainWindow(void)
{
	Watcher.cancel(); delete ui;
}

QMap<int, double> MainWindow::RunWorkerBlock(QPair<int, boost::function<int (void)>> Params)
{
	QMap<int, double> Result;

	for (int i = 0; i < Params.first; ++i)
	{
		++Result[Params.second()];
	}

	return Result;
}

boost::function<int (void)> MainWindow::GetWorkerEngine(int Min, int Max, int Seed, int Distribution, int Engine)
{
	qDebug() << Seed;

	switch (Distribution)
	{
		case 0:
			switch (Engine)
			{
				case 0: return boost::bind(boost::random::uniform_int_distribution<>(Min, Max), boost::random::rand48(Seed));
				case 1: return boost::bind(boost::random::uniform_int_distribution<>(Min, Max), boost::random::taus88(Seed));
				case 2: return boost::bind(boost::random::uniform_int_distribution<>(Min, Max), boost::random::ranlux4(Seed));
				case 3: return boost::bind(boost::random::uniform_int_distribution<>(Min, Max), boost::random::mt11213b(Seed));
			}
		break;

		case 1:
			switch (Engine)
			{
				case 0: return boost::bind(boost::random::bernoulli_distribution<>(), boost::random::rand48(Seed));
				case 1: return boost::bind(boost::random::bernoulli_distribution<>(), boost::random::taus88(Seed));
				case 2: return boost::bind(boost::random::bernoulli_distribution<>(), boost::random::ranlux4(Seed));
				case 3: return boost::bind(boost::random::bernoulli_distribution<>(), boost::random::mt11213b(Seed));
			}
		break;

		case 2:
			switch (Engine)
			{
				case 0: return boost::bind(boost::random::poisson_distribution<>(), boost::random::rand48(Seed));
				case 1: return boost::bind(boost::random::poisson_distribution<>(), boost::random::taus88(Seed));
				case 2: return boost::bind(boost::random::poisson_distribution<>(), boost::random::ranlux4(Seed));
				case 3: return boost::bind(boost::random::poisson_distribution<>(), boost::random::mt11213b(Seed));
			}
		break;

		case 3:
			switch (Engine)
			{
				case 0: return boost::bind(boost::random::normal_distribution<>(Min, Max), boost::random::rand48(Seed));
				case 1: return boost::bind(boost::random::normal_distribution<>(Min, Max), boost::random::taus88(Seed));
				case 2: return boost::bind(boost::random::normal_distribution<>(Min, Max), boost::random::ranlux4(Seed));
				case 3: return boost::bind(boost::random::normal_distribution<>(Min, Max), boost::random::mt11213b(Seed));
			}
		break;

		case 4:
			switch (Engine)
			{
				case 0: return boost::bind(boost::random::triangle_distribution<>(Min, Max), boost::random::rand48(Seed));
				case 1: return boost::bind(boost::random::triangle_distribution<>(Min, Max), boost::random::taus88(Seed));
				case 2: return boost::bind(boost::random::triangle_distribution<>(Min, Max), boost::random::ranlux4(Seed));
				case 3: return boost::bind(boost::random::triangle_distribution<>(Min, Max), boost::random::mt11213b(Seed));
			}
		break;
	}

	return boost::bind(boost::random::uniform_int_distribution<>(Min, Max), boost::random::rand48(Seed));
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

		case Normal:

		break;

		case Extended:

		break;
	}
}

void MainWindow::PlotRangeChanged(const QCPRange& New, const QCPRange& Old)
{
	if (New.lower < 0) ui->Plot->yAxis->setRange(0, Old.size());
}

void MainWindow::PlotReadyResult(int ID)
{
	const QTime Now = QTime::currentTime();

	auto Result = Watcher.resultAt(ID);
	double Sum = 0.0;

	for (auto i = Result.begin(); i != Result.end(); ++i)
	{
		Results[i.key()] += i.value();
	}

	for (const auto& S : Results) Sum += S;
	for (auto& S : Results) S = 100.0 * S / Sum;

	if (Last.msecsTo(Now) > 100)
	{
		Bars->setData(Results.keys().toVector(),
				    Results.values().toVector());

		ui->Plot->replot();

		Last = Now;
	}
}

void MainWindow::RangeSpinChanged(void)
{
	ui->Max->setMinimum(ui->Min->value() + 1);
	ui->Min->setMaximum(ui->Max->value() - 1);
}

void MainWindow::RunActionClicked(void)
{
	SwitchUiStatus(Locked);

	if (Watcher.isPaused())
	{
		Watcher.resume();
	}
	else
	{
		const int Samples = ui->Samples->value();
		const int Threads = ui->Threads->value();

		const int Engine = ui->Engine->currentIndex();
		const int Distribution = ui->Distribution->currentIndex();

		const int Min = ui->Min->value();
		const int Max = ui->Max->value();

		const int Iters = MAX_ITERS;

		QList<QPair<int, boost::function<int (void)>>> Jobs;

		srand(ui->Seed->value());

		if (Samples / Threads > Iters)
		{
			int Left = Samples - Iters;

			for (int i = 1; i < (Samples / Iters); ++i, Left -= Iters)
			{
				Jobs.append(QPair<int, boost::function<int (void)>>(Iters, GetWorkerEngine(Min, Max, rand(), Distribution, Engine)));
			}

			Jobs.append(QPair<int, boost::function<int (void)>>(Left, GetWorkerEngine(Min, Max, rand(), Distribution, Engine)));
		}
		else
		{
			if (Samples >= Threads) for (int i = 0; i < Threads; ++i)
			{
				Jobs.append(QPair<int, boost::function<int (void)>>(Samples / Threads, GetWorkerEngine(Min, Max, rand(), Distribution, Engine)));
			}
			else Jobs.append(QPair<int, boost::function<int (void)>>(0, GetWorkerEngine(Min, Max, rand(), Distribution, Engine)));

			Jobs.last().first += Samples - Threads * Jobs.last().first;
		}

		if (ui->Autoclear->isChecked()) ClearActionClicked();

		QThreadPool::globalInstance()->setMaxThreadCount(Threads);

		Watcher.setFuture(QtConcurrent::mapped(Jobs, boost::bind(&MainWindow::RunWorkerBlock, this, _1)));
	}
}

void MainWindow::StopActionClicked(void)
{
	SwitchUiStatus(Unlocked);
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
