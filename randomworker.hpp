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

#ifndef RANDOMWORKER_HPP
#define RANDOMWORKER_HPP

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QTime>
#include <QMap>

#include <boost/bind.hpp>
#include <boost/random.hpp>
#include <boost/function.hpp>

class RandomWorker : public QObject
{

		Q_OBJECT

	Q_PROPERTY(STATUS getStatus READ getStatus NOTIFY onStatusChanged)

	public: enum STATUS
	{
		Stopped,
		Running,
		Paused
	};

	private:

		boost::function<int (void)> Generator;

		QMutex Locker;

		STATUS Status = Stopped;

		unsigned Loops = 1000;
		unsigned Iters = 100;
		unsigned Left = 0;
		unsigned Current = 0;
		unsigned Sleep = 0;

	public:

		explicit RandomWorker(QObject* Parent = nullptr);

		boost::function<int (void)> getGenerator(double P1, double P2, double P3, int Seed, int Distribution, int Engine);

		void setGenerator(double P1, double P2, double P3, int Seed, int Distribution, int Engine);
		void setGenerator(const boost::function<int (void)>& Functional);

		void setSamples(unsigned Samples);
		void setBlocks(unsigned Blocks);
		void setSleep(unsigned Msecs);

		STATUS getStatus(void) const;

	private slots:

		void runLoop(void);

	public slots:

		void startProgress(void);
		void pauseProgress(void);
		void resumeProgress(void);
		void stopProgress(void);

	signals:

		void onResultsReady(const QMap<int, int>&);

		void onStatusChanged(STATUS);

		void onProgressBegin(int, int);
		void onProgressUpdate(int);
		void onProgressEnd(void);

		void onLoopRequest(void);

};

#endif // RANDOMWORKER_HPP
