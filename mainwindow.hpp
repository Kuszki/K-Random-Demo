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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <QDebug>

#include <qcustomplot.h>
#include <limits.h>

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{

		Q_OBJECT

	private: enum UiStatus
	{
		Unlocked,
		Locked,
		Normal,
		Extended
	};

	private:

		Ui::MainWindow* ui = nullptr;

		QList<QObject*> runLocked;
		QList<QObject*> stopLocked;

		QCPBars* Bars = nullptr;

	private:

		void SwitchUiStatus(UiStatus Status);

	public:

		explicit MainWindow(QWidget* Parent = nullptr);
		virtual ~MainWindow(void) override;

	private slots:

		void PlotRangeChanged(const QCPRange& New, const QCPRange& Old);

		void RunActionClicked(void);
		void StopActionClicked(void);
		void PauseActionClicked(void);
		void AdjustActionClicked(void);

};

#endif // MAINWINDOW_HPP
