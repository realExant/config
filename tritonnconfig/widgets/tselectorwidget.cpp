#include "tselectorwidget.h"
#include "ui_tselectorwidget.h"

#include "dialogs/tdataselectdialog.h"

TSelectorWidget::TSelectorWidget(TTritonn *tritonn, QWidget *parent)
    : TWidget(tritonn, parent),
      ui(new Ui::TSelectorWidget) {

    ui->setupUi(this);

    ui->spinBox_inputCount->setRange(2, 4);

    //General
    ui->comboBox_type->addItem(toString(UnitType::Temperature     ), static_cast<uint32_t>(UnitType::Temperature     ));
    ui->comboBox_type->addItem(toString(UnitType::Pressure        ), static_cast<uint32_t>(UnitType::Pressure        ));
    ui->comboBox_type->addItem(toString(UnitType::Density         ), static_cast<uint32_t>(UnitType::Density         ));
    ui->comboBox_type->addItem(toString(UnitType::ViscosityDyn    ), static_cast<uint32_t>(UnitType::ViscosityDyn    ));
    ui->comboBox_type->addItem(toString(UnitType::ViscosityKin    ), static_cast<uint32_t>(UnitType::ViscosityKin    ));
    ui->comboBox_type->addItem(toString(UnitType::QuantityMass    ), static_cast<uint32_t>(UnitType::QuantityMass    ));
    ui->comboBox_type->addItem(toString(UnitType::QuantityVolume  ), static_cast<uint32_t>(UnitType::QuantityVolume  ));
    ui->comboBox_type->addItem(toString(UnitType::FlowrateMass    ), static_cast<uint32_t>(UnitType::FlowrateMass    ));
    ui->comboBox_type->addItem(toString(UnitType::FlowrateVolume  ), static_cast<uint32_t>(UnitType::FlowrateVolume  ));
    ui->comboBox_type->addItem(toString(UnitType::KFactorsMass    ), static_cast<uint32_t>(UnitType::KFactorsMass    ));
    ui->comboBox_type->addItem(toString(UnitType::KFactorsVolume  ), static_cast<uint32_t>(UnitType::KFactorsVolume  ));
    ui->comboBox_type->addItem(toString(UnitType::Frequency       ), static_cast<uint32_t>(UnitType::Frequency       ));
    ui->comboBox_type->addItem(toString(UnitType::Time            ), static_cast<uint32_t>(UnitType::Time            ));
    ui->comboBox_type->addItem(toString(UnitType::Dimless         ), static_cast<uint32_t>(UnitType::Dimless         ));
    ui->comboBox_type->addItem(toString(UnitType::Percent         ), static_cast<uint32_t>(UnitType::Percent         ));
    ui->comboBox_type->addItem(toString(UnitType::Impulse         ), static_cast<uint32_t>(UnitType::Impulse         ));
    ui->comboBox_type->addItem(toString(UnitType::Miliampere      ), static_cast<uint32_t>(UnitType::Miliampere      ));
    ui->comboBox_type->addItem(toString(UnitType::Voltage         ), static_cast<uint32_t>(UnitType::Voltage         ));
    ui->comboBox_type->addItem(toString(UnitType::CoeffExpansion  ), static_cast<uint32_t>(UnitType::CoeffExpansion  ));
    ui->comboBox_type->addItem(toString(UnitType::CoeffCompression), static_cast<uint32_t>(UnitType::CoeffCompression));
    ui->comboBox_type->addItem(toString(UnitType::WaterCutMass    ), static_cast<uint32_t>(UnitType::WaterCutMass    ));
    ui->comboBox_type->addItem(toString(UnitType::WaterCutVolume  ), static_cast<uint32_t>(UnitType::WaterCutVolume  ));

    ui->comboBox_type->model()->sort(0, Qt::AscendingOrder);
    updateComboBoxUnits(static_cast<UnitType>(ui->comboBox_type->currentData().toUInt()));

    ui->comboBox_behaviorOnFail->addItem(toString(TSelector::M_NothingToDo    ), TSelector::M_NothingToDo    );
    ui->comboBox_behaviorOnFail->addItem(toString(TSelector::M_SelectKeypad   ), TSelector::M_SelectKeypad   );
    ui->comboBox_behaviorOnFail->addItem(toString(TSelector::M_SelectNextInput), TSelector::M_SelectNextInput);
    ui->comboBox_behaviorOnFail->addItem(toString(TSelector::M_SelectPrevInput), TSelector::M_SelectPrevInput);

    ui->label_inputLinkEdit_1->setText("-");
    ui->label_inputLinkEdit_2->setText("-");
    ui->label_inputLinkEdit_3->setText("-");
    ui->label_inputLinkEdit_4->setText("-");
    ui->label_faultLinkEdit_1->setText("-");
    ui->label_faultLinkEdit_2->setText("-");
    ui->label_faultLinkEdit_3->setText("-");
    ui->label_faultLinkEdit_4->setText("-");

    ui->label_inputLinkEdit_1->setTextFormat(Qt::RichText);
    ui->label_inputLinkEdit_2->setTextFormat(Qt::RichText);
    ui->label_inputLinkEdit_3->setTextFormat(Qt::RichText);
    ui->label_inputLinkEdit_4->setTextFormat(Qt::RichText);
    ui->label_faultLinkEdit_1->setTextFormat(Qt::RichText);
    ui->label_faultLinkEdit_2->setTextFormat(Qt::RichText);
    ui->label_faultLinkEdit_3->setTextFormat(Qt::RichText);
    ui->label_faultLinkEdit_4->setTextFormat(Qt::RichText);

    ui->label_faultLink_1->setEnabled(false);
    ui->label_faultLink_2->setEnabled(false);
    ui->label_faultLink_3->setEnabled(false);
    ui->label_faultLink_4->setEnabled(false);

    setValidators();
    connectSignals();
}

TSelectorWidget::TSelectorWidget(TSelector *object, TTritonn *tritonn, QWidget *parent)
    : TSelectorWidget(tritonn, parent) {

    setCalcObject(object);
}

TSelectorWidget::~TSelectorWidget() {
    _calcObject = nullptr;
    delete ui; ui = nullptr;
}

void TSelectorWidget::connectSignals() {
    connectGeneralSignals();
    connectDescriptionSignals();
    connectInputSignals();
    connectFaultSignals();
}

void TSelectorWidget::connectGeneralSignals() {
    connect(ui->lineEdit_name, &QLineEdit::textEdited, [this] (const QString &text) {
        _calcObject->setOpcTagName(text);
        emit linkNameChanged();
        emit dataChanged();
    });

    connect(ui->comboBox_type, QOverload<int>::of(&QComboBox::activated), [this] (int index) {
        const QSignalBlocker b1(ui->comboBox_type);
        const QSignalBlocker b2(ui->comboBox_units);

        UnitType newType = static_cast<UnitType>(ui->comboBox_type->itemData(index).toUInt());
        UnitType currentType = _calcObject->output()->value()->getUnitType();

        if (newType != currentType && _calcObject->getCountLinkedInputs() > 0) {
            QMessageBox::StandardButton result = QMessageBox::question(this, tr("Engineer Units..."), tr("Changing the type of engineering units will reset the input links. Continue?"));

            if (result == QMessageBox::Yes) {
                _calcObject->clearLinkedInputs();
                updateInputs();
                updateFaults();
            }
            else {
                ui->comboBox_type->setCurrentText(toString(currentType));
                return;
            }
        }
        _calcObject->setUnit(toUnit(newType));
        updateComboBoxUnits(newType);
        updateRepresentation();
        emit dataChanged();
    });

    connect(ui->lineEdit_keypad, &QLineEdit::textChanged, [this] (const QString &text) {
        _calcObject->keypad()->value()->setData(text.toDouble());
        emit dataChanged();
    });

    connect(ui->comboBox_units, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int index) {
        Unit unit = static_cast<Unit>(ui->comboBox_units->itemData(index).toUInt());
        _calcObject->setUnit(unit);
        updateRepresentation();
        emit dataChanged();
    });

    connect(ui->spinBox_inputCount, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int i) {
        _calcObject->setLastActiveInput(static_cast<TSelectorInput::Number>(i));

        ui->spinBox_initialInput->setMaximum(static_cast<int>(_calcObject->getLastActiveInput()));
        ui->spinBox_initialInput->setValue(static_cast<int>(_calcObject->getInitialInput()));

        updateInitialInput();
        updateInputs();
        updateFaults();

        emit dataChanged();
    });

    connect(ui->spinBox_initialInput, QOverload<int>::of(&QSpinBox::valueChanged), [this] (int i) {
        TSelectorInput::Number number = static_cast<TSelectorInput::Number>(i);
        _calcObject->setInitialInput(number);

        updateInitialInput();
        emit dataChanged();
    });

    connect(ui->comboBox_behaviorOnFail, QOverload<int>::of(&QComboBox::currentIndexChanged), [this] (int index) {
        TSelector::Mode behavior = static_cast<TSelector::Mode>(ui->comboBox_behaviorOnFail->itemData(index).toUInt());
        _calcObject->setBehaviorOnFail(behavior);
        emit dataChanged();
    });

    connect(ui->checkBox_useFaultLinks, &QCheckBox::stateChanged, [this] (int state) {
        if (state == Qt::Checked)
            _calcObject->activateFaultLinks();
        else
            _calcObject->deactivateFaultLinks();

        updateFaults();
        emit dataChanged();
    });

    connect(ui->checkBox_suppressEventMessages, &QCheckBox::stateChanged, [this] (int state) {
        _calcObject->setSuppressEventMessages(state == Qt::Checked);
        emit dataChanged();
    });
}

void TSelectorWidget::connectDescriptionSignals() {
    connect(ui->comboBox_lang, &QComboBox::currentTextChanged, [this] (const QString &lang) {
        QSignalBlocker b0(ui->plainTextEdit_description);

        ui->plainTextEdit_description->setPlainText(_calcObject->getDescription(lang));
    });

    connect(ui->plainTextEdit_description, &QPlainTextEdit::textChanged, [this] () {
        QString lang = ui->comboBox_lang->currentText();
        _calcObject->setDescrtiption(ui->plainTextEdit_description->toPlainText(), lang);
        emit dataChanged();
    });
}

void TSelectorWidget::connectInputSignals() {
    connect(ui->label_inputLinkEdit_1, &QLabel::linkActivated, [this] (const QString &) {
        emit linkActivated(_calcObject->input(TSelectorInput::N_1)->link()->bindedObject());
    });

    connect(ui->label_inputLinkEdit_2, &QLabel::linkActivated, [this] (const QString &) {
        emit linkActivated(_calcObject->input(TSelectorInput::N_2)->link()->bindedObject());
    });

    connect(ui->label_inputLinkEdit_3, &QLabel::linkActivated, [this] (const QString &) {
        emit linkActivated(_calcObject->input(TSelectorInput::N_3)->link()->bindedObject());
    });

    connect(ui->label_inputLinkEdit_4, &QLabel::linkActivated, [this] (const QString &) {
        emit linkActivated(_calcObject->input(TSelectorInput::N_4)->link()->bindedObject());
    });
}

void TSelectorWidget::connectFaultSignals() {
    connect(ui->label_faultLinkEdit_1, &QLabel::linkActivated, [this] (const QString &) {
        emit linkActivated(_calcObject->input(TSelectorInput::N_1)->faultLink()->bindedObject());
    });

    connect(ui->label_faultLinkEdit_2, &QLabel::linkActivated, [this] (const QString &) {
        emit linkActivated(_calcObject->input(TSelectorInput::N_2)->faultLink()->bindedObject());
    });

    connect(ui->label_faultLinkEdit_3, &QLabel::linkActivated, [this] (const QString &) {
        emit linkActivated(_calcObject->input(TSelectorInput::N_3)->faultLink()->bindedObject());
    });

    connect(ui->label_faultLinkEdit_4, &QLabel::linkActivated, [this] (const QString &) {
        emit linkActivated(_calcObject->input(TSelectorInput::N_4)->faultLink()->bindedObject());
    });
}

void TSelectorWidget::updateInputs() {
    updateInput_1();
    updateInput_2();
    updateInput_3();
    updateInput_4();
}

void TSelectorWidget::updateInput_1() {
    auto link      = _calcObject->input(TSelectorInput::N_1)->link();
    auto enable    = (TSelectorInput::N_1 <= _calcObject->getLastActiveInput());
    auto label     = ui->label_inputLink_1;
    auto labelEdit = ui->label_inputLinkEdit_1;
    auto button    = ui->pushButton_inputLinkSelect_1;

    TWidget::updateLink(enable, link, label, labelEdit, button);
}

void TSelectorWidget::updateInput_2() {
    auto link      = _calcObject->input(TSelectorInput::N_2)->link();
    auto enable    = (TSelectorInput::N_2 <= _calcObject->getLastActiveInput());
    auto label     = ui->label_inputLink_2;
    auto labelEdit = ui->label_inputLinkEdit_2;
    auto button    = ui->pushButton_inputLinkSelect_2;

    TWidget::updateLink(enable, link, label, labelEdit, button);
}

void TSelectorWidget::updateInput_3() {
    auto link      = _calcObject->input(TSelectorInput::N_3)->link();
    auto enable    = (TSelectorInput::N_3 <= _calcObject->getLastActiveInput());
    auto label     = ui->label_inputLink_3;
    auto labelEdit = ui->label_inputLinkEdit_3;
    auto button    = ui->pushButton_inputLinkSelect_3;

    TWidget::updateLink(enable, link, label, labelEdit, button);
}

void TSelectorWidget::updateInput_4() {
    auto link      = _calcObject->input(TSelectorInput::N_4)->link();
    auto enable    = (TSelectorInput::N_4 <= _calcObject->getLastActiveInput());
    auto label     = ui->label_inputLink_4;
    auto labelEdit = ui->label_inputLinkEdit_4;
    auto button    = ui->pushButton_inputLinkSelect_4;

    TWidget::updateLink(enable, link, label, labelEdit, button);
}

void TSelectorWidget::updateFaults() {
    updateFault_1();
    updateFault_2();
    updateFault_3();
    updateFault_4();
}

void TSelectorWidget::updateFault_1() {
    auto link           = _calcObject->input(TSelectorInput::N_1)->faultLink();
    auto enable         = (TSelectorInput::N_1 <= _calcObject->getLastActiveInput());
    auto label          = ui->label_faultLink_1;
    auto labelEdit      = ui->label_faultLinkEdit_1;
    auto button         = ui->pushButton_faultLinkSelect_1;
    auto buttonAutobind = ui->pushButton_autobindFaultLink_1;

    TWidget::updateFault(enable, link, label, labelEdit, button, buttonAutobind);
}

void TSelectorWidget::updateFault_2() {
    auto link           = _calcObject->input(TSelectorInput::N_2)->faultLink();
    auto enable         = (TSelectorInput::N_2 <= _calcObject->getLastActiveInput());
    auto label          = ui->label_faultLink_2;
    auto labelEdit      = ui->label_faultLinkEdit_2;
    auto button         = ui->pushButton_faultLinkSelect_2;
    auto buttonAutobind = ui->pushButton_autobindFaultLink_2;

    TWidget::updateFault(enable, link, label, labelEdit, button, buttonAutobind);
}

void TSelectorWidget::updateFault_3() {
    auto link           = _calcObject->input(TSelectorInput::N_3)->faultLink();
    auto enable         = (TSelectorInput::N_3 <= _calcObject->getLastActiveInput());
    auto label          = ui->label_faultLink_3;
    auto labelEdit      = ui->label_faultLinkEdit_3;
    auto button         = ui->pushButton_faultLinkSelect_3;
    auto buttonAutobind = ui->pushButton_autobindFaultLink_3;

    TWidget::updateFault(enable, link, label, labelEdit, button, buttonAutobind);
}

void TSelectorWidget::updateFault_4() {
    auto link           = _calcObject->input(TSelectorInput::N_4)->faultLink();
    auto enable         = (TSelectorInput::N_4 <= _calcObject->getLastActiveInput());
    auto label          = ui->label_faultLink_4;
    auto labelEdit      = ui->label_faultLinkEdit_4;
    auto button         = ui->pushButton_faultLinkSelect_4;
    auto buttonAutobind = ui->pushButton_autobindFaultLink_4;

    TWidget::updateFault(enable, link, label, labelEdit, button, buttonAutobind);
}

void TSelectorWidget::updateComboBoxUnits(UnitType type) {
    {//scope for blocker
        const QSignalBlocker blocker(ui->comboBox_units);
        ui->comboBox_units->clear();
    }

    for (int i = 0; i < unitDictionary()->recordCount(); ++i) {
        TUnitFormat *format = unitDictionary()->recordAt(i);
        if (type == format->getUnitType())
            ui->comboBox_units->addItem(dictionary()->record(format->getUnit())->getText(), format->getUnit());
    }

    if (UnitType::Any == type || UnitType::Dimless == type)
        ui->comboBox_units->setEnabled(false);
    else
        ui->comboBox_units->setEnabled(true);
}

void TSelectorWidget::setCalcObject(TSelector *object) {
    assert(object != nullptr && "object is nullptr");

    if (object->getUnitType() == UnitType::Any)
        object->setUnit(Unit::Dimless);

    const QSignalBlocker blocker(this);

    _calcObject = object;

    setGeneral();
    setDescription();
    setInputs();
    setFaults();

    updateRepresentation();
}

void TSelectorWidget::setGeneral() {
    const QSignalBlocker b1(ui->lineEdit_name);
    const QSignalBlocker b2(ui->comboBox_type);
    const QSignalBlocker b3(ui->comboBox_units);
    const QSignalBlocker b4(ui->lineEdit_keypad);
    const QSignalBlocker b5(ui->spinBox_inputCount);
    const QSignalBlocker b6(ui->spinBox_initialInput);
    const QSignalBlocker b7(ui->comboBox_behaviorOnFail);
    const QSignalBlocker b8(ui->checkBox_useFaultLinks);
    const QSignalBlocker b9(ui->checkBox_suppressEventMessages);

    ui->lineEdit_name->setText(_calcObject->getOpcTagName());
    ui->comboBox_type->setCurrentText(toString(_calcObject->output()->value()->getUnitType()));

    if (_calcObject->hasOutputLinks())
        ui->comboBox_type->setEnabled(false);
    else
        ui->comboBox_type->setEnabled(true);

    UnitType type = static_cast<UnitType>(ui->comboBox_type->currentData().toUInt());
    updateComboBoxUnits(type);

    ui->comboBox_units->setCurrentText(dictionary()->record(_calcObject->getUnit())->getText());

    ui->spinBox_inputCount->setValue(static_cast<int>(_calcObject->getLastActiveInput()));
    ui->spinBox_initialInput->setValue(static_cast<int>(_calcObject->getInitialInput()));
    updateInitialInput();

    ui->comboBox_behaviorOnFail->setCurrentText(toString(_calcObject->getBehaviorOnFail()));
    ui->checkBox_suppressEventMessages->setChecked(_calcObject->isSuppressedEventMessages());
}

void TSelectorWidget::setDescription() {
    const QSignalBlocker b1(ui->comboBox_lang);
    const QSignalBlocker b2(ui->plainTextEdit_description);

    ui->comboBox_lang->clear();

    for (int i = 0; i < dictionary()->langCount(); ++i)
        ui->comboBox_lang->addItem(dictionary()->langAt(i));

    QString lang = dictionary()->getDefaultLang();
    ui->comboBox_lang->setCurrentText(lang);
    ui->plainTextEdit_description->setPlainText(_calcObject->getDescription(lang));
}

void TSelectorWidget::setInputs() {
    const QSignalBlocker b1(ui->label_inputLink_1);
    const QSignalBlocker b2(ui->label_inputLinkEdit_1);
    const QSignalBlocker b3(ui->pushButton_inputLinkSelect_1);
    const QSignalBlocker b4(ui->pushButton_autobindFaultLink_1);

    const QSignalBlocker b5(ui->label_inputLink_2);
    const QSignalBlocker b6(ui->label_inputLinkEdit_2);
    const QSignalBlocker b7(ui->pushButton_inputLinkSelect_2);
    const QSignalBlocker b8(ui->pushButton_autobindFaultLink_2);

    const QSignalBlocker b9(ui->label_inputLink_3);
    const QSignalBlocker b10(ui->label_inputLinkEdit_3);
    const QSignalBlocker b11(ui->pushButton_inputLinkSelect_3);
    const QSignalBlocker b12(ui->pushButton_autobindFaultLink_3);

    const QSignalBlocker b13(ui->label_inputLink_4);
    const QSignalBlocker b14(ui->label_inputLinkEdit_4);
    const QSignalBlocker b15(ui->pushButton_inputLinkSelect_4);
    const QSignalBlocker b16(ui->pushButton_autobindFaultLink_4);

    updateInputs();
}

void TSelectorWidget::setFaults() {
    const QSignalBlocker b1(ui->label_faultLink_1);
    const QSignalBlocker b2(ui->label_faultLinkEdit_1);
    const QSignalBlocker b3(ui->pushButton_faultLinkSelect_1);

    const QSignalBlocker b4(ui->label_faultLink_2);
    const QSignalBlocker b5(ui->label_faultLinkEdit_2);
    const QSignalBlocker b6(ui->pushButton_faultLinkSelect_2);

    const QSignalBlocker b7(ui->label_faultLink_3);
    const QSignalBlocker b8(ui->label_faultLinkEdit_3);
    const QSignalBlocker b9(ui->pushButton_faultLinkSelect_3);

    const QSignalBlocker b10(ui->label_faultLink_4);
    const QSignalBlocker b11(ui->label_faultLinkEdit_4);
    const QSignalBlocker b12(ui->pushButton_faultLinkSelect_4);

    ui->checkBox_useFaultLinks->setChecked(_calcObject->hasActivedFaultLinks());
    updateFaults();
}

void TSelectorWidget::setValidators() {
    setValidatorName();
    setValidatorValue();
}

void TSelectorWidget::setValidatorName() {
    QValidator *validator = new QRegExpValidator(QRegExp("[A-Za-z0-9_]*"), this);
    validator->setLocale(QLocale::c());
    ui->lineEdit_name->setValidator(validator);
}

void TSelectorWidget::setValidatorValue() {
    _valueValidator->setNotation(QDoubleValidator::StandardNotation);
    _valueValidator->setLocale(QLocale::c());

    ui->lineEdit_keypad->setValidator(_valueValidator);
}

void TSelectorWidget::selectInputLink(TSelectorInput::Number number) {
    TDataSelectDialog *dialog = new TDataSelectDialog(TDataSelectDialog::SM_Single, this);
    dialog->setBrowseObject(tritonn(), _calcObject);
    dialog->activateUnitTypeFilter(_calcObject->output()->value()->getUnitType());
    dialog->activateDirectionFilter(LinkDirection::Output);
    dialog->setEnabledFilterGroup(false);

    connect(dialog, &TDataSelectDialog::accepted, [dialog, this, number] () {
        dialog->deleteLater();

        if (dialog->selectedData().count() > 0) {
            if (TDataAbstract *data = dialog->selectedData().first()) {
                _calcObject->input(number)->link()->bind(data);

                switch (number) {
                case TSelectorInput::N_1: updateInput_1(); break;
                case TSelectorInput::N_2: updateInput_2(); break;
                case TSelectorInput::N_3: updateInput_3(); break;
                case TSelectorInput::N_4: updateInput_4(); break;
                }

                emit dataChanged();
            }
        }
    });

    dialog->show();
}

void TSelectorWidget::selectFaultLink(TSelectorInput::Number number) {
    TDataSelectDialog *dialog = new TDataSelectDialog(TDataSelectDialog::SM_Single, this);
    dialog->setBrowseObject(tritonn(), _calcObject);
    dialog->activateTagNameFilter(_calcObject->input(number)->fault()->getOpcTagName());
    dialog->setEnabledFilterGroup(false);

    connect(dialog, &TDataSelectDialog::accepted, [dialog, this, number] () {
        dialog->deleteLater();

        if (dialog->selectedData().count() > 0) {
            if (TDataAbstract *data = dialog->selectedData().first()) {
                _calcObject->input(number)->faultLink()->bind(data);
                updateFaults();
                emit dataChanged();
            }
        }
    });

    dialog->show();
}

void TSelectorWidget::updateRepresentation() {
    TUnitFormat *uf = unitDictionary()->format(_calcObject->getUnit());

    _valueValidator->setNotation(static_cast<QDoubleValidator::Notation>(uf->getNotation()));
    _valueValidator->setDecimals(uf->getPrecision());

    ui->lineEdit_keypad->setText(unitDictionary()->representation(_calcObject->keypad()));
}

void TSelectorWidget::updateInitialInput() {
    ui->groupBox_input_1->setTitle(tr("Input 1"));
    ui->groupBox_input_2->setTitle(tr("Input 2"));
    ui->groupBox_input_3->setTitle(tr("Input 3"));
    ui->groupBox_input_4->setTitle(tr("Input 4"));

    switch (_calcObject->getInitialInput()) {
    case TSelectorInput::N_1: ui->groupBox_input_1->setTitle(tr("Input 1 (initial)")); break;
    case TSelectorInput::N_2: ui->groupBox_input_2->setTitle(tr("Input 2 (initial)")); break;
    case TSelectorInput::N_3: ui->groupBox_input_3->setTitle(tr("Input 3 (initial)")); break;
    case TSelectorInput::N_4: ui->groupBox_input_4->setTitle(tr("Input 4 (initial)")); break;
    }
}

void TSelectorWidget::on_pushButton_inputLinkSelect_1_released() {
    selectInputLink(TSelectorInput::N_1);
}

void TSelectorWidget::on_pushButton_inputLinkSelect_2_released() {
    selectInputLink(TSelectorInput::N_2);
}

void TSelectorWidget::on_pushButton_inputLinkSelect_3_released() {
    selectInputLink(TSelectorInput::N_3);
}

void TSelectorWidget::on_pushButton_inputLinkSelect_4_released() {
    selectInputLink(TSelectorInput::N_4);
}

void TSelectorWidget::on_pushButton_faultLinkSelect_1_released() {
    selectFaultLink(TSelectorInput::N_1);
}

void TSelectorWidget::on_pushButton_faultLinkSelect_2_released() {
    selectFaultLink(TSelectorInput::N_2);
}

void TSelectorWidget::on_pushButton_faultLinkSelect_3_released() {
    selectFaultLink(TSelectorInput::N_3);
}

void TSelectorWidget::on_pushButton_faultLinkSelect_4_released() {
    selectFaultLink(TSelectorInput::N_4);
}

void TSelectorWidget::on_pushButton_translate_released() {
    assert(_calcObject != nullptr && "_calcObject is nullptr");

    QString lang = ui->comboBox_lang->currentText();
    emit translateActivated(_calcObject->getDescriptionId(), lang);
}

void TSelectorWidget::on_pushButton_autobindFaultLink_1_released() {
    auto input = _calcObject->input(TSelectorInput::N_1);

    if (input->faultLink()->bindByLink(input->link())) {
        updateFault_1();
        emit dataChanged();
    }
}

void TSelectorWidget::on_pushButton_autobindFaultLink_2_released() {
    auto input = _calcObject->input(TSelectorInput::N_2);

    if (input->faultLink()->bindByLink(input->link())) {
        updateFault_2();
        emit dataChanged();
    }
}

void TSelectorWidget::on_pushButton_autobindFaultLink_3_released() {
    auto input = _calcObject->input(TSelectorInput::N_3);

    if (input->faultLink()->bindByLink(input->link())) {
        updateFault_3();
        emit dataChanged();
    }
}

void TSelectorWidget::on_pushButton_autobindFaultLink_4_released() {
    auto input = _calcObject->input(TSelectorInput::N_4);

    if (input->faultLink()->bindByLink(input->link())) {
        updateFault_4();
        emit dataChanged();
    }
}

void TSelectorWidget::on_pushButton_formatting_released() {
    Unit unit = static_cast<Unit>(ui->comboBox_units->currentData().toUInt());
    emit formattingActivated(unit);
}
