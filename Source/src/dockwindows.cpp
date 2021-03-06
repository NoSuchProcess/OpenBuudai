////////////////////////////////////////////////////////////////////////////////
//
//  OpenHantek
//  dockwindows.cpp
//
//  Copyright (C) 2010  Oliver Haag
//  oliver.haag@gmail.com
//
//  This program is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by the Free
//  Software Foundation, either version 3 of the License, or (at your option)
//  any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//  more details.
//
//  You should have received a copy of the GNU General Public License along with
//  this program.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////


#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QLabel>


#include "dockwindows.h"

#include "settings.h"
#include "helper.h"


////////////////////////////////////////////////////////////////////////////////
// class HorizontalDock
/// \brief Initializes the horizontal axis docking window.
/// \param settings The target settings object.
/// \param parent The parent widget.
/// \param flags Flags for the window manager.
HorizontalDock::HorizontalDock(DsoSettings *settings, QWidget *parent, Qt::WindowFlags flags) : QDockWidget(tr("Horizontal"), parent, flags) {
	this->settings = settings;
	
	// Initialize lists for comboboxes
	this->timebaseSteps         << 1e-8 << 2e-8 << 5e-8 << 1e-7 << 2e-7 << 5e-7
			<< 1e-6 << 2e-6 << 5e-6 << 1e-5 << 2e-5 << 5e-5 << 1e-4 << 2e-4 << 5e-4
            << 1e-3 << 2e-3 << 5e-3 << 1e-2 << 2e-2 << 5e-2 << 1e-1 << 2e-1;

            /// Disable timebase >= 500ms
            /// << 5e-1
            /// <<  1e0 <<  2e0 <<  5e0 <<  1e1 <<  2e1 <<  5e1 <<  6e1 << 12e1 << 24e1
            /// <<  6e2 << 12e2 << 24e2 << 36e2;  ///< Timebase steps in seconds/div
	for (QList<double>::iterator timebase = this->timebaseSteps.begin(); timebase != this->timebaseSteps.end(); ++timebase)
		this->timebaseStrings << Helper::valueToString(*timebase, Helper::UNIT_SECONDS, 0);
	this->frequencybaseSteps
			<<  1.0 <<  2.0 <<  5.0 <<  1e1 <<  2e1 <<  5e1 <<  1e2 <<  2e2 <<  5e2
			<<  1e3 <<  2e3 <<  5e3 <<  1e4 <<  2e4 <<  4e4 <<  1e5 <<  2e5 <<  5e5
			<<  1e6 <<  2e6 <<  5e6 <<  1e7; ///< Frequencybase steps in Hz/div
	for (QList<double>::iterator frequencybase = this->frequencybaseSteps.begin(); frequencybase != this->frequencybaseSteps.end(); ++frequencybase)
		this->frequencybaseStrings << Helper::valueToString(*frequencybase, Helper::UNIT_HERTZ, 0);
	
	// Initialize elements
	this->timebaseLabel = new QLabel(tr("Timebase"));
	this->timebaseComboBox = new QComboBox();
	this->timebaseComboBox->addItems(this->timebaseStrings);
	
	this->frequencybaseLabel = new QLabel(tr("Frequencybase"));
	this->frequencybaseComboBox = new QComboBox();
	this->frequencybaseComboBox->addItems(this->frequencybaseStrings);
	
	this->formatLabel = new QLabel(tr("Format"));
	this->formatComboBox = new QComboBox();
	for (int format = Dso::GRAPHFORMAT_TY; format < Dso::GRAPHFORMAT_COUNT; format++)
		this->formatComboBox->addItem(Dso::graphFormatString((Dso::GraphFormat) format));
	
	this->dockLayout = new QGridLayout();
	this->dockLayout->setColumnMinimumWidth(0, 64);
	this->dockLayout->setColumnStretch(1, 1);
	this->dockLayout->addWidget(this->timebaseLabel, 0, 0);
	this->dockLayout->addWidget(this->timebaseComboBox, 0, 1);
	this->dockLayout->addWidget(this->frequencybaseLabel, 1, 0);
	this->dockLayout->addWidget(this->frequencybaseComboBox, 1, 1);
	this->dockLayout->addWidget(this->formatLabel, 2, 0);
	this->dockLayout->addWidget(this->formatComboBox, 2, 1);
	
	this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	
	this->dockWidget = new QWidget();
	this->dockWidget->setLayout(this->dockLayout);
	this->setWidget(this->dockWidget);
	
	// Connect signals and slots
	connect(this->frequencybaseComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(frequencybaseSelected(int)));
	connect(this->timebaseComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(timebaseSelected(int)));
	connect(this->formatComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(formatSelected(int)));
	
	// Set values
	this->setTimebase(this->settings->scope.horizontal.timebase);
	this->setFrequencybase(this->settings->scope.horizontal.frequencybase);
	this->setFormat(this->settings->scope.horizontal.format);
}

/// \brief Cleans up everything.
HorizontalDock::~HorizontalDock() {
}

/// \brief Don't close the dock, just hide it.
/// \param event The close event that should be handled.
void HorizontalDock::closeEvent(QCloseEvent *event) {
	this->hide();
	
	event->accept();
}

/// \brief Changes the frequencybase if the new value is supported.
/// \param frequencybase The frequencybase in hertz.
/// \return Index of frequencybase-value, -1 on error.
int HorizontalDock::setFrequencybase(double frequencybase) {
	int index = this->frequencybaseSteps.indexOf(frequencybase);
	
	if (index != -1)
		this->frequencybaseComboBox->setCurrentIndex(index);
	
	return index;
}

/// \brief Changes the timebase if the new value is supported.
/// \param timebase The timebase in seconds.
/// \return Index of timebase-value, -1 on error.
int HorizontalDock::setTimebase(double timebase) {
	int index = this->timebaseSteps.indexOf(timebase);
	
	if (index != -1)
		this->timebaseComboBox->setCurrentIndex(index);
	
	return index;
}

/// \brief Changes the format if the new value is supported.
/// \param format The format for the horizontal axis.
/// \return Index of format-value, -1 on error.
int HorizontalDock::setFormat(Dso::GraphFormat format) {
	if (format >= Dso::GRAPHFORMAT_TY && format <= Dso::GRAPHFORMAT_XY) {
		this->formatComboBox->setCurrentIndex(format);
		return format;
	}
	
	return -1;
}

/// \brief Called when the frequencybase combo box changes it's value.
/// \param index The index of the combo box item.
void HorizontalDock::frequencybaseSelected(int index) {
	this->settings->scope.horizontal.frequencybase = this->frequencybaseSteps.at(index);
	emit frequencybaseChanged(this->settings->scope.horizontal.frequencybase);
}

/// \brief Called when the timebase combo box changes it's value.
/// \param index The index of the combo box item.
void HorizontalDock::timebaseSelected(int index) {
	this->settings->scope.horizontal.timebase = this->timebaseSteps.at(index);
	emit timebaseChanged(this->settings->scope.horizontal.timebase);
}

/// \brief Called when the format combo box changes it's value.
/// \param index The index of the combo box item.
void HorizontalDock::formatSelected(int index) {
	this->settings->scope.horizontal.format = (Dso::GraphFormat) index;
	emit formatChanged(this->settings->scope.horizontal.format);
}


////////////////////////////////////////////////////////////////////////////////
// class TriggerDock
/// \brief Initializes the trigger settings docking window.
/// \param settings The target settings object.
/// \param specialTriggers The names of the special trigger sources.
/// \param parent The parent widget.
/// \param flags Flags for the window manager.
TriggerDock::TriggerDock(DsoSettings *settings, const QStringList *specialTriggers, QWidget *parent, Qt::WindowFlags flags) : QDockWidget(tr("Trigger"), parent, flags) {
	this->settings = settings;
	
	// Initialize lists for comboboxes
	for (unsigned int channel = 0; channel < this->settings->scope.physicalChannels; channel++)
		this->sourceStandardStrings << tr("CH%1").arg(channel + 1);
	this->sourceSpecialStrings << *specialTriggers;
	
	// Initialize elements
	this->modeLabel = new QLabel(tr("Mode"));
	this->modeComboBox = new QComboBox();
	for (int mode = Dso::TRIGGERMODE_AUTO; mode < Dso::TRIGGERMODE_COUNT; mode++)
		this->modeComboBox->addItem(Dso::triggerModeString((Dso::TriggerMode) mode));

	this->slopeLabel = new QLabel(tr("Slope"));
	this->slopeComboBox = new QComboBox();
	for (int slope = Dso::SLOPE_POSITIVE; slope < Dso::SLOPE_COUNT; slope++)
		this->slopeComboBox->addItem(Dso::slopeString((Dso::Slope) slope));

	this->sourceLabel = new QLabel(tr("Source"));
	this->sourceComboBox = new QComboBox();
	this->sourceComboBox->addItems(this->sourceStandardStrings);
	this->sourceComboBox->addItems(this->sourceSpecialStrings);

	this->dockLayout = new QGridLayout();
	this->dockLayout->setColumnMinimumWidth(0, 64);
	this->dockLayout->setColumnStretch(1, 1);
	this->dockLayout->addWidget(this->modeLabel, 0, 0);
	this->dockLayout->addWidget(this->modeComboBox, 0, 1);
	this->dockLayout->addWidget(this->sourceLabel, 1, 0);
	this->dockLayout->addWidget(this->sourceComboBox, 1, 1);
	this->dockLayout->addWidget(this->slopeLabel, 2, 0);
	this->dockLayout->addWidget(this->slopeComboBox, 2, 1);

	this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	
	this->dockWidget = new QWidget();
	this->dockWidget->setLayout(this->dockLayout);
	this->setWidget(this->dockWidget);
	
	// Connect signals and slots
	connect(this->modeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(modeSelected(int)));
	connect(this->slopeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slopeSelected(int)));
	connect(this->sourceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sourceSelected(int)));
	
	// Set values
	this->setMode(this->settings->scope.trigger.mode);
	this->setSlope(this->settings->scope.trigger.slope);
	this->setSource(this->settings->scope.trigger.special, this->settings->scope.trigger.source);
}

/// \brief Cleans up everything.
TriggerDock::~TriggerDock() {
}

/// \brief Don't close the dock, just hide it 
/// \param event The close event that should be handled.
void TriggerDock::closeEvent(QCloseEvent *event) {
	this->hide();
	
	event->accept();
}

/// \brief Changes the trigger mode if the new mode is supported.
/// \param mode The trigger mode.
/// \return Index of mode-value, -1 on error.
int TriggerDock::setMode(Dso::TriggerMode mode) {
	if (mode >= Dso::TRIGGERMODE_AUTO && mode <=Dso:: TRIGGERMODE_SINGLE) {
		this->modeComboBox->setCurrentIndex(mode);
		return mode;
	}
	
	return -1;
}

/// \brief Changes the trigger slope if the new slope is supported.
/// \param slope The trigger slope.
/// \return Index of slope-value, -1 on error.
int TriggerDock::setSlope(Dso::Slope slope) {
	if (slope >= Dso::SLOPE_POSITIVE && slope <= Dso::SLOPE_NEGATIVE) {
		this->slopeComboBox->setCurrentIndex(slope);
		return slope;
	}
	
	return -1;
}

/// \brief Changes the trigger source if the new source is supported.
/// \param special true for a special channel (EXT, ...) as trigger source.
/// \param id The number of the channel, that should be used as trigger.
/// \return Index of source item, -1 on error.
int TriggerDock::setSource(bool special, unsigned int id) {
	if ((!special && id >= (unsigned int) this->sourceStandardStrings.count()) ||
			(special && id >= (unsigned int) this->sourceSpecialStrings.count()))
		return -1;
	
	int index = id;
	if (special)
		index += this->sourceStandardStrings.count();
	this->sourceComboBox->setCurrentIndex(index);
	
	return index;
}

/// \brief Called when the mode combo box changes it's value.
/// \param index The index of the combo box item.
void TriggerDock::modeSelected(int index) {
	this->settings->scope.trigger.mode = (Dso::TriggerMode) index;
	emit modeChanged(this->settings->scope.trigger.mode);
}

/// \brief Called when the slope combo box changes it's value.
/// \param index The index of the combo box item.
void TriggerDock::slopeSelected(int index) {
	this->settings->scope.trigger.slope = (Dso::Slope) index;
	emit slopeChanged(this->settings->scope.trigger.slope);
}

/// \brief Called when the source combo box changes it's value.
/// \param index The index of the combo box item.
void TriggerDock::sourceSelected(int index) {
	unsigned int id = index;
	bool special = false;
	
	if (id >= (unsigned int) this->sourceStandardStrings.count()) {
		id -= this->sourceStandardStrings.count();
		special = true;
	}
	
	this->settings->scope.trigger.source = id;
	this->settings->scope.trigger.special = special;
	emit sourceChanged(special, id);
}


////////////////////////////////////////////////////////////////////////////////
// class SpectrumDock
/// \brief Initializes the spectrum view docking window.
/// \param settings The target settings object.
/// \param parent The parent widget.
/// \param flags Flags for the window manager.
SpectrumDock::SpectrumDock(DsoSettings *settings, QWidget *parent, Qt::WindowFlags flags)
	: QDockWidget(tr("Spectrum"), parent, flags) {
	this->settings = settings;
	
	// Initialize lists for comboboxes
	this->magnitudeSteps  <<  1e0 <<  2e0 <<  3e0 <<  6e0
			<<  1e1 <<  2e1 <<  3e1 <<  6e1 <<  1e2 <<  2e2 <<  3e2 <<  6e2; ///< Magnitude steps in dB/div
	for (QList<double>::iterator magnitude = this->magnitudeSteps.begin(); magnitude != this->magnitudeSteps.end(); ++magnitude)
		this->magnitudeStrings << Helper::valueToString(*magnitude, Helper::UNIT_DECIBEL, 0);
	
	// Initialize elements
	for (int channel = 0; channel < this->settings->scope.voltage.count(); channel++) {
		this->magnitudeComboBox.append(new QComboBox());
		this->magnitudeComboBox[channel]->addItems(this->magnitudeStrings);
		
		this->usedCheckBox.append(new QCheckBox(this->settings->scope.voltage[channel].name));
	}
	
	this->dockLayout = new QGridLayout();
	this->dockLayout->setColumnMinimumWidth(0, 64);
	this->dockLayout->setColumnStretch(1, 1);
	for (int channel = 0; channel < this->settings->scope.voltage.count(); channel++) {
		this->dockLayout->addWidget(this->usedCheckBox[channel], channel, 0);
		this->dockLayout->addWidget(this->magnitudeComboBox[channel], channel, 1);
	}

	this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	
	this->dockWidget = new QWidget();
	this->dockWidget->setLayout(this->dockLayout);
	this->setWidget(this->dockWidget);
	
	// Connect signals and slots
	for (int channel = 0; channel < this->settings->scope.voltage.count(); channel++) {
		connect(this->magnitudeComboBox[channel], SIGNAL(currentIndexChanged(int)), this, SLOT(magnitudeSelected(int)));
		connect(this->usedCheckBox[channel], SIGNAL(toggled(bool)), this, SLOT(usedSwitched(bool)));
	}
	
	// Set values
	for (int channel = 0; channel < this->settings->scope.voltage.count(); channel++) {
		this->setMagnitude(channel, this->settings->scope.spectrum[channel].magnitude);
		this->setUsed(channel, this->settings->scope.spectrum[channel].used);
	}
}

/// \brief Cleans up everything.
SpectrumDock::~SpectrumDock() {
}

/// \brief Don't close the dock, just hide it 
/// \param event The close event that should be handled.
void SpectrumDock::closeEvent(QCloseEvent *event) {
	this->hide();
	
	event->accept();
}

/// \brief Sets the magnitude for a channel.
/// \param channel The channel, whose magnitude should be set.
/// \param magnitude The magnitude in dB.
/// \return Index of magnitude-value, -1 on error.
int SpectrumDock::setMagnitude(int channel, double magnitude) {
	if (channel < 0 || channel >= this->settings->scope.voltage.count())
		return -1;
	
	int index = this->magnitudeSteps.indexOf(magnitude);
	if (index != -1)
		this->magnitudeComboBox[channel]->setCurrentIndex(index);
	
	return index;
}

/// \brief Enables/disables a channel.
/// \param channel The channel, that should be enabled/disabled.
/// \param used True if the channel should be enabled, false otherwise.
/// \return Index of channel, -1 on error.
int SpectrumDock::setUsed(int channel, bool used) {
	if (channel >= 0 && channel < this->settings->scope.voltage.count()) {
		this->usedCheckBox[channel]->setChecked(used);
		return channel;
	}
	
	return -1;
}

/// \brief Called when the source combo box changes it's value.
/// \param index The index of the combo box item.
void SpectrumDock::magnitudeSelected(int index) {
	int channel;
	
	// Which combobox was it?
	for (channel = 0; channel < this->settings->scope.voltage.count(); channel++)
		if (this->sender() == this->magnitudeComboBox[channel])
			break;
	
	// Send signal if it was one of the comboboxes
	if (channel < this->settings->scope.voltage.count()) {
		this->settings->scope.spectrum[channel].magnitude = this->magnitudeSteps.at(index);
		emit magnitudeChanged(channel, this->settings->scope.spectrum[channel].magnitude);
	}
}

/// \brief Called when the used checkbox is switched.
/// \param checked The check-state of the checkbox.
void SpectrumDock::usedSwitched(bool checked) {
	int channel;
	
	// Which checkbox was it?
	for (channel = 0; channel < this->settings->scope.voltage.count(); channel++)
		if (this->sender() == this->usedCheckBox[channel])
			break;
	
	// Send signal if it was one of the checkboxes
	if (channel < this->settings->scope.voltage.count()) {
		this->settings->scope.spectrum[channel].used = checked;
		emit usedChanged(channel, checked);
	}
}


////////////////////////////////////////////////////////////////////////////////
// class VoltageDock
/// \brief Initializes the vertical axis docking window.
/// \param settings The target settings object.
/// \param parent The parent widget.
/// \param flags Flags for the window manager.
VoltageDock::VoltageDock(DsoSettings *settings, QWidget *parent, Qt::WindowFlags flags) : QDockWidget(tr("Voltage"), parent, flags) {
	this->settings = settings;
	
	// Initialize lists for comboboxes
	for (int coupling = Dso::COUPLING_AC; coupling < Dso::COUPLING_COUNT; coupling++)
		this->couplingStrings.append(Dso::couplingString((Dso::Coupling) coupling));
	
	for (int mode = Dso::MATHMODE_1ADD2; mode < Dso::MATHMODE_COUNT; mode++)
		this->modeStrings.append(Dso::mathModeString((Dso::MathMode) mode));
	
	this->gainSteps             << 1e-2 << 2e-2 << 5e-2 << 1e-1 << 2e-1 << 5e-1
			<<  1e0 <<  2e0 <<  5e0;          ///< Voltage steps in V/div
	for (QList<double>::iterator gain = this->gainSteps.begin(); gain != this->gainSteps.end(); gain++)
		this->gainStrings << Helper::valueToString(*gain, Helper::UNIT_VOLTS, 0);
	
	// Initialize elements
	for (int channel = 0; channel < this->settings->scope.voltage.count(); channel++) {
		this->miscComboBox.append(new QComboBox());
		if (channel < (int) this->settings->scope.physicalChannels)
			this->miscComboBox[channel]->addItems(this->couplingStrings);
		else
			this->miscComboBox[channel]->addItems(this->modeStrings);
		
		this->gainComboBox.append(new QComboBox());
		this->gainComboBox[channel]->addItems(this->gainStrings);
		
		this->usedCheckBox.append(new QCheckBox(this->settings->scope.voltage[channel].name));
	}
	
	this->dockLayout = new QGridLayout();
	this->dockLayout->setColumnMinimumWidth(0, 64);
	this->dockLayout->setColumnStretch(1, 1);
	for (int channel = 0; channel < this->settings->scope.voltage.count(); channel++) {
		this->dockLayout->addWidget(this->usedCheckBox[channel], channel * 2, 0);
		this->dockLayout->addWidget(this->gainComboBox[channel], channel * 2, 1);
		this->dockLayout->addWidget(this->miscComboBox[channel], channel * 2 + 1, 1);
	}

	this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	
	this->dockWidget = new QWidget();
	this->dockWidget->setLayout(this->dockLayout);
	this->setWidget(this->dockWidget);
	
	// Connect signals and slots
	for (int channel = 0; channel < this->settings->scope.voltage.count(); channel++) {
		connect(this->gainComboBox[channel], SIGNAL(currentIndexChanged(int)), this, SLOT(gainSelected(int)));
		connect(this->miscComboBox[channel], SIGNAL(currentIndexChanged(int)), this, SLOT(miscSelected(int)));
		connect(this->usedCheckBox[channel], SIGNAL(toggled(bool)), this, SLOT(usedSwitched(bool)));
	}
	
	// Set values
	for (int channel = 0; channel < this->settings->scope.voltage.count(); channel++) {
		if (channel < (int) this->settings->scope.physicalChannels)
			this->setCoupling(channel, (Dso::Coupling) this->settings->scope.voltage[channel].misc);
		else
			this->setMode((Dso::MathMode) this->settings->scope.voltage[channel].misc);
		this->setGain(channel, this->settings->scope.voltage[channel].gain);
		this->setUsed(channel, this->settings->scope.voltage[channel].used);
	}
}

/// \brief Cleans up everything.
VoltageDock::~VoltageDock() {
}

/// \brief Don't close the dock, just hide it 
/// \param event The close event that should be handled.
void VoltageDock::closeEvent(QCloseEvent *event) {
	this->hide();
	
	event->accept();
}

/// \brief Sets the coupling for a channel.
/// \param channel The channel, whose coupling should be set.
/// \param coupling The coupling-mode.
/// \return Index of coupling-mode, -1 on error.
int VoltageDock::setCoupling(int channel, Dso::Coupling coupling) {
	if (coupling < Dso::COUPLING_AC || coupling > Dso::COUPLING_GND)
		return -1;
	if (channel < 0 || channel >= (int) this->settings->scope.physicalChannels)
		return -1;
	
	this->miscComboBox[channel]->setCurrentIndex(coupling);
	return coupling;
}

/// \brief Sets the gain for a channel.
/// \param channel The channel, whose gain should be set.
/// \param gain The gain in volts.
/// \return Index of gain-value, -1 on error.
int VoltageDock::setGain(int channel, double gain) {
	if (channel < 0 || channel >= this->settings->scope.voltage.count())
		return -1;
	
	int index = this->gainSteps.indexOf(gain);
	if (index != -1)
		this->gainComboBox[channel]->setCurrentIndex(index);
	
	return index;
}

/// \brief Sets the mode for the math channel.
/// \param mode The math-mode.
/// \return Index of math-mode, -1 on error.
int VoltageDock::setMode(Dso::MathMode mode) {
	if (mode >= Dso::MATHMODE_1ADD2 && mode <= Dso::MATHMODE_2SUB1) {
		this->miscComboBox[this->settings->scope.physicalChannels]->setCurrentIndex(mode);
		return mode;
	}
	
	return -1;
}

/// \brief Enables/disables a channel.
/// \param channel The channel, that should be enabled/disabled.
/// \param used True if the channel should be enabled, false otherwise.
/// \return Index of channel, -1 on error.
int VoltageDock::setUsed(int channel, bool used) {
	if (channel >= 0 && channel < this->settings->scope.voltage.count()) {
		this->usedCheckBox[channel]->setChecked(used);
		return channel;
	}
	
	return -1;
}

/// \brief Called when the gain combo box changes it's value.
/// \param index The index of the combo box item.
void VoltageDock::gainSelected(int index) {
	int channel;
	
	// Which combobox was it?
	for (channel = 0; channel < this->settings->scope.voltage.count(); channel++)
		if (this->sender() == this->gainComboBox[channel])
			break;
	
	// Send signal if it was one of the comboboxes
	if (channel < this->settings->scope.voltage.count()) {
		this->settings->scope.voltage[channel].gain = this->gainSteps.at(index);
		
		emit gainChanged(channel, this->settings->scope.voltage[channel].gain);
	}
}

/// \brief Called when the misc combo box changes it's value.
/// \param index The index of the combo box item.
void VoltageDock::miscSelected(int index) {
	int channel;
	
	// Which combobox was it?
	for (channel = 0; channel < this->settings->scope.voltage.count(); channel++)
		if (this->sender() == this->miscComboBox[channel])
			break;
	
	// Send signal if it was one of the comboboxes
	if (channel < this->settings->scope.voltage.count()) {
		this->settings->scope.voltage[channel].misc = index;
		if (channel < (int) this->settings->scope.physicalChannels)
			emit couplingChanged(channel, (Dso::Coupling) this->settings->scope.voltage[channel].misc);
		else
			emit modeChanged((Dso::MathMode) this->settings->scope.voltage[channel].misc);
	}
}

/// \brief Called when the used checkbox is switched.
/// \param checked The check-state of the checkbox.
void VoltageDock::usedSwitched(bool checked) {
	int channel;
	
	// Which checkbox was it?
	for (channel = 0; channel < this->settings->scope.voltage.count(); channel++)
		if (this->sender() == this->usedCheckBox[channel])
			break;
	
	// Send signal if it was one of the checkboxes
	if (channel < this->settings->scope.voltage.count()) {
		this->settings->scope.voltage[channel].used = checked;
		emit usedChanged(channel, checked);
	}
}
