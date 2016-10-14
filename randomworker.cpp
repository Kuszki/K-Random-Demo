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

#include "randomworker.hpp"

RandomWorker::RandomWorker(QObject* Parent)
	: QObject(Parent), Locker(QMutex::Recursive)
{
	connect(this, &RandomWorker::onLoopRequest, this, &RandomWorker::runLoop, Qt::QueuedConnection);
}

void RandomWorker::runLoop(void)
{
	QMutexLocker autoLocker(&Locker);

	if (Left && Status == Running)
	{
		const QTime Start = QTime::currentTime();
		QMap<int, int> Results;

		unsigned i;

		for (i = 0; Left && i < Iters; ++i, --Left)
		{
			++Results[Generator()];
		}

		const int Diff = Sleep - Start.msecsTo(QTime::currentTime());

		emit onResultsReady(Results);
		emit onProgressUpdate(Current += i);

		if (Diff > 0 && Diff < 1001) thread()->msleep(Diff);

		emit onLoopRequest();
	}
	else if (!Left) stopProgress();
}

RandomWorker::STATUS RandomWorker::getStatus(void) const
{
	return Status;
}

void RandomWorker::startProgress(void)
{
	QMutexLocker autoLocker(&Locker);

	switch (Status)
	{
		case Stopped:
			Left = Loops;
			Current = 0;
		case Paused:
			Status = Running;

			emit onStatusChanged(Status);
			emit onProgressBegin(0, Loops);
			emit onLoopRequest();

		break;

		default: break;
	}
}

void RandomWorker::pauseProgress(void)
{
	QMutexLocker autoLocker(&Locker);

	if (Status == Running)
	{
		emit onStatusChanged(Status = Paused);
	}
}

void RandomWorker::resumeProgress(void)
{
	QMutexLocker autoLocker(&Locker);

	if (Status == Paused)
	{
		emit onStatusChanged(Status = Running);
		emit onProgressResume();
		emit onLoopRequest();
	}
}

void RandomWorker::stopProgress(void)
{
	QMutexLocker autoLocker(&Locker);

	if (Status != Stopped)
	{
		emit onStatusChanged(Status = Stopped);
		emit onProgressEnd();
	}
}

void RandomWorker::setGenerator(double P1, double P2, double P3, int Seed, int Distribution, int Engine)
{
	Locker.lock();
	Generator = getGenerator(P1, P2, P3, Seed, Distribution, Engine);
	Locker.unlock();
}

void RandomWorker::setGenerator(const boost::function<int (void)>& Functional)
{
	Locker.lock();
	Generator = Functional;
	Locker.unlock();
}

void RandomWorker::setSamples(unsigned Samples)
{
	Loops = Samples;
}

void RandomWorker::setBlocks(unsigned Blocks)
{
	Iters = Blocks;
}

void RandomWorker::setSleep(unsigned Msecs)
{
	Locker.lock();
	Sleep = Msecs;
	Locker.unlock();
}

boost::function<int (void)> RandomWorker::getGenerator(double P1, double P2, double P3, int Seed, int Distribution, int Engine)
{
	switch (Distribution)
	{
		case 0:
			switch (Engine)
			{
				case 0: return boost::bind(boost::random::uniform_int_distribution<>(P1, P2), boost::random::rand48(Seed));
				case 1: return boost::bind(boost::random::uniform_int_distribution<>(P1, P2), boost::random::taus88(Seed));
				case 2: return boost::bind(boost::random::uniform_int_distribution<>(P1, P2), boost::random::ranlux4(Seed));
				case 3: return boost::bind(boost::random::uniform_int_distribution<>(P1, P2), boost::random::mt11213b(Seed));
			}
		break;

		case 1:
			switch (Engine)
			{
				case 0: return boost::bind(boost::random::exponential_distribution<>(P1), boost::random::rand48(Seed));
				case 1: return boost::bind(boost::random::exponential_distribution<>(P1), boost::random::taus88(Seed));
				case 2: return boost::bind(boost::random::exponential_distribution<>(P1), boost::random::ranlux4(Seed));
				case 3: return boost::bind(boost::random::exponential_distribution<>(P1), boost::random::mt11213b(Seed));
			}
		break;

		case 2:
			switch (Engine)
			{
				case 0: return boost::bind(boost::random::poisson_distribution<>(P1), boost::random::rand48(Seed));
				case 1: return boost::bind(boost::random::poisson_distribution<>(P1), boost::random::taus88(Seed));
				case 2: return boost::bind(boost::random::poisson_distribution<>(P1), boost::random::ranlux4(Seed));
				case 3: return boost::bind(boost::random::poisson_distribution<>(P1), boost::random::mt11213b(Seed));
			}
		break;

		case 3:
			switch (Engine)
			{
				case 0: return boost::bind(boost::random::normal_distribution<>(P1, P2), boost::random::rand48(Seed));
				case 1: return boost::bind(boost::random::normal_distribution<>(P1, P2), boost::random::taus88(Seed));
				case 2: return boost::bind(boost::random::normal_distribution<>(P1, P2), boost::random::ranlux4(Seed));
				case 3: return boost::bind(boost::random::normal_distribution<>(P1, P2), boost::random::mt11213b(Seed));
			}
		break;

		case 4:
			switch (Engine)
			{
				case 0: return boost::bind(boost::random::triangle_distribution<>(P1, P2, P3), boost::random::rand48(Seed));
				case 1: return boost::bind(boost::random::triangle_distribution<>(P1, P2, P3), boost::random::taus88(Seed));
				case 2: return boost::bind(boost::random::triangle_distribution<>(P1, P2, P3), boost::random::ranlux4(Seed));
				case 3: return boost::bind(boost::random::triangle_distribution<>(P1, P2, P3), boost::random::mt11213b(Seed));
			}
		break;
	}

	return boost::bind(boost::random::uniform_int_distribution<>(P1, P2), boost::random::rand48(Seed));
}
