#include "tselector.h"

#include "tcalcobjects.h"

#include "xml/txml.h"

TRITONN_BEGIN_NAMESPACE

TSelectorInput::TSelectorInput(const QString &opcTagName, Unit unit, TObject *parent)
    : TInputValue(opcTagName, unit, parent) {

    _fault = new TUInt32(this, OPCTAG_FAULT, LinkDirection::Input, Access::ReadOnly, Unit::Dimless);
    _fault->setInfo(QObject::tr("Input fault status"));

    _faultLink = new TLink(fault()->getOpcTagName(), fault(), this);
    _faultLink->setVirtual(true);
}

void TSelectorInput::validate(TValidateEvent &event) const {
    TInputValue::validate(event);

    if (faultLink()->isEnabled() && !faultLink()->bindedObject())
        event.append(this, TValidateMessage::T_Error, QObject::tr("Fault link '%1' is enabled and has not valid reference!").arg(getOpcTagName()));

    if (faultLink()->isBinded() && !link()->isBinded())
        event.append(this, TValidateMessage::T_Error, QObject::tr("Fault link is binded but has not a Value link for '%1'!").arg(getOpcTagName()));
}

void TSelectorInput::setUnitUpdater(TUnitUpdater *updater) {
    _value->setUnitUpdater(updater);
}

TSelector::TSelector(TCalcObjects *parent)
    : TCalcObjectAbstract(TCalcObjectAbstract::OT_Selector, parent) {

    setObjectName("Selector");
    setOpcTagNameByType("Sel");
    setInfo(QObject::tr("Selector object"));

    _select     = new TInt16(this,  OPCTAG_SELECT,      LinkDirection::NoDirection, Access::ReadWrite, Unit::Dimless);
    _inputCount = new TUInt16(this, OPCTAG_INPUT_COUNT, LinkDirection::NoDirection, Access::ReadOnly,  Unit::Dimless);
    _mode       = new TUInt16(this, OPCTAG_MODE,        LinkDirection::NoDirection, Access::ReadWrite, Unit::Dimless);
    _setup      = new TUInt16(this, OPCTAG_SETUP,       LinkDirection::NoDirection, Access::ReadOnly,  Unit::Dimless);
    _fault      = new TUInt32(this, OPCTAG_FAULT,       LinkDirection::NoDirection, Access::ReadOnly,  Unit::Dimless);

    _output = new TOutputValue(OPCTAG_OUTPUT, Unit::Any, this);
    _keypad = new TInternalValue(OPCTAG_KEYPAD, Access::ReadWrite, Unit::Any, this);

    for (uint i = TSelectorInput::N_1; i <= TSelectorInput::N_4; ++i)
        appendInput(new TSelectorInput(QString("Input_%1").arg(i), Unit::Any, this));

    setBehaviorOnFail(M_NothingToDo);

    setUnitUpdater();
    updateEnabledInputs();
}

TSelector::TSelector(const QString &opcTagName, TCalcObjects *parent)
    : TSelector(parent) {

    setOpcTagName(opcTagName);
}

void TSelector::validate(TValidateEvent &event) const {
    TCalcObjectAbstract::validate(event);

    if (TSelectorInput::N_1 == getLastActiveInput()  &&
            (getBehaviorOnFail() == M_SelectNextInput || getBehaviorOnFail() == M_SelectPrevInput))
        event.interrupt(this, TValidateMessage::T_Error, QObject::tr("The Behavior on Fail cannot be '%1' or '%2' with one active input link!\n"
                                                                     "Please сhange the behavior.").arg(M_SelectNextInput).arg(M_SelectPrevInput));
}

bool TSelector::hasActivedFaultLinks() const {
    for (uint i = TSelectorInput::N_1; i <= getLastActiveInput(); ++i)
        if (input(static_cast<TSelectorInput::Number>(i))->faultLink()->isEnabled())
            return true;
    return false;
}

void TSelector::activateFaultLinks() {
    for (uint i = TSelectorInput::N_1; i <= getLastActiveInput(); ++i)
        input(static_cast<TSelectorInput::Number>(i))->faultLink()->setVirtual(false);

    for (uint i = getLastActiveInput() + 1; i <= TSelectorInput::N_4; ++i)
        input(static_cast<TSelectorInput::Number>(i))->faultLink()->setVirtual(true);
}

void TSelector::deactivateFaultLinks() {
    for (uint i = TSelectorInput::N_1; i <= getLastActiveInput(); ++i)
        input(static_cast<TSelectorInput::Number>(i))->faultLink()->setVirtual(true);

    for (uint i = getLastActiveInput() + 1; i <= TSelectorInput::N_4; ++i)
        input(static_cast<TSelectorInput::Number>(i))->faultLink()->setVirtual(false);
}

TSelector::Mode TSelector::getBehaviorOnFail() const {
    return static_cast<Mode>(_mode->getData());
}

void TSelector::setBehaviorOnFail(TSelector::Mode behavior) {
    _mode->setData(static_cast<uint8_t>(behavior));
}

bool TSelector::isSuppressedEventMessages() const {
    return _setup->testFlag(SF_NoEvent);
}

void TSelector::setSuppressEventMessages(bool supress) {
    _setup->setFlag(SF_NoEvent, supress);
}

TSelectorInput *TSelector::input(TSelectorInput::Number number) const {
    return nodeAt<TSelectorInput *>(static_cast<int>(number) - 1);
}

void TSelector::setLastActiveInput(TSelectorInput::Number number) {
    _lastActiveInput = number;

    if (_initialInput > _lastActiveInput)
        _initialInput = _lastActiveInput;

    updateEnabledInputs();
}

Unit TSelector::getUnit() const {
    return _unitsUpdater->lastUpdatedUnit();
}

void TSelector::setUnit(Unit unit) {
    _unitsUpdater->send(unit);
}

void TSelector::setUnitUpdater() {
    _unitsUpdater = std::make_unique<TUnitUpdater>();

    _output->setUnitUpdater(_unitsUpdater.get());
    _keypad->setUnitUpdater(_unitsUpdater.get());

    for (uint i = TSelectorInput::N_1; i <= TSelectorInput::N_4; ++i)
        input(static_cast<TSelectorInput::Number>(i))->setUnitUpdater(_unitsUpdater.get());
}

int TSelector::getCountLinkedInputs() const {
    int count = 0;
    for (uint i = TSelectorInput::N_1; i <= TSelectorInput::N_4; ++i)
        if (input(static_cast<TSelectorInput::Number>(i))->link()->isBinded())
            count++;
    return count;
}

void TSelector::clearLinkedInputs() {
    for (uint i = TSelectorInput::N_1; i <= TSelectorInput::N_4; ++i) {
        input(static_cast<TSelectorInput::Number>(i))->link()->unbind();
        input(static_cast<TSelectorInput::Number>(i))->faultLink()->unbind();
    }
}

void TSelector::serialize(TXmlElementRef xml_parent, const TXmlVersion &xml_version) const {
    if (xml_version == TXmlVersion::currentVersion) {
        auto xml_selector = xml_parent.insertChildElement(getXmlTagName());

        TXmlFlags mode;
        mode.append("NOCHANGE", getBehaviorOnFail() == M_NothingToDo);
        mode.append("ERROR",    getBehaviorOnFail() == M_SelectKeypad);
        mode.append("NEXT",     getBehaviorOnFail() == M_SelectNextInput);
        mode.append("PREV",     getBehaviorOnFail() == M_SelectPrevInput);

        TXmlFlags setup;
        setup.append("OFF",     _setup->testFlag(SF_Off));
        setup.append("NOEVENT", _setup->testFlag(SF_NoEvent));
        //Флаг SF_Multi не указывать в конфигурации

        xml_selector.setAttribute("name",        getOpcTagName());
        xml_selector.setAttribute("mode",        mode);
        xml_selector.setAttribute("setup",       setup);
        xml_selector.setAttribute("select",      static_cast<int>(getInitialInput()) - 1);
        xml_selector.setAttribute("description", getDescriptionId());

        auto xml_inputs = xml_selector.insertChildElement("inputs");

        for (uint i = TSelectorInput::N_1; i <= _lastActiveInput; ++i) {
            auto input = this->input(static_cast<TSelectorInput::Number>(i));
            if (input->link()->isBinded())
                input->link()->serialize(xml_inputs, xml_version, true);
        }

        if (hasActivedFaultLinks()) {
            auto xml_faults = xml_selector.insertChildElement("faults");

            for (uint i = TSelectorInput::N_1; i <= _lastActiveInput; ++i) {
                auto input = this->input(static_cast<TSelectorInput::Number>(i));
                if (input->faultLink()->isBinded())
                    input->faultLink()->serialize(xml_faults, xml_version, false);
            }
        }

        keypad()->serialize(xml_selector, xml_version);
    }
}

void TSelector::deserialize(const TXmlElementRef xml_this, const TXmlVersion &xml_version) {
    if (xml_version == TXmlVersion::currentVersion) {
        if (xml_this.isEmpty())
            return;

        setOpcTagName(xml_this.getAttributeString("name"));

        int initialInput = xml_this.getAttributeInt("select") + 1;
        setInitialInput(static_cast<TSelectorInput::Number>(initialInput));

        setDescrtiptionId(xml_this.getAttributeUInt("description"));

        TXmlFlags setup = xml_this.getAttributeFlags("setup");
        TXmlFlags mode  = xml_this.getAttributeFlags("mode");

        _setup->setFlag(SF_Off,     setup.isEnabled("OFF"));
        _setup->setFlag(SF_NoEvent, setup.isEnabled("NOEVENT"));
        //Флаг SF_Multi не указывать в конфигурации

        if (mode.isEnabled("NOCHANGE")) _mode->setData(M_NothingToDo);
        if (mode.isEnabled("ERROR"))    _mode->setData(M_SelectKeypad);
        if (mode.isEnabled("NEXT"))     _mode->setData(M_SelectNextInput);
        if (mode.isEnabled("PREV"))     _mode->setData(M_SelectPrevInput);

        auto xml_inputs = xml_this.firstChildElement("inputs");

        int i_link = 0;
        int i_faultLink = 0;

        XML_FOR_ALL(xml_link, xml_inputs) {
            i_link++;

            if (i_link > static_cast<int>(TSelectorInput::N_4))
                throw TXmlError(xml_inputs.getLineNum(), xml_inputs.getName(),
                                QString("Count of Value link (=%1) is exceeds max value (=4)").arg(i_link));

            auto input = this->input(static_cast<TSelectorInput::Number>(i_link));
            input->link()->setVirtual(false);
            input->link()->deserialize(xml_link, xml_version);
        }

        setLastActiveInput(static_cast<TSelectorInput::Number>(i_link));

        if (i_link < static_cast<int>(TSelectorInput::N_2))
            throw TXmlError(xml_inputs.getLineNum(), xml_inputs.getName(),
                            QString("Count of Value link (=%1) is exceeds min value (=2)").arg(i_link));

        if (xml_this.hasFirstChildElement("faults")) {
            auto xml_faults = xml_this.firstChildElement("faults");

            XML_FOR_ALL(xml_faultLink, xml_faults) {
                i_faultLink++;

                if (i_faultLink > static_cast<int>(TSelectorInput::N_4))
                    throw TXmlError(xml_faults.getLineNum(), xml_faults.getName(),
                                    QString("Count of Fault link (=%1) is exceeds max value (=4)").arg(i_faultLink));

                auto input = this->input(static_cast<TSelectorInput::Number>(i_faultLink));
                input->faultLink()->setVirtual(false);
                input->faultLink()->deserialize(xml_faultLink, xml_version);
            }

            if (i_link != i_faultLink)
                throw TXmlError(xml_faults.getLineNum(), xml_faults.getName(),
                                QString("Value link count (=%1) is not equal Fault link count (=%2)").arg(i_link).arg(i_faultLink));
        }

        auto xml_keypad = xml_this.firstChildElement(keypad()->getOpcTagName());
        keypad()->deserialize(xml_keypad, xml_version);
    }
}

void TSelector::updateEnabledInputs() {
    for (uint i = 1; i <= _lastActiveInput; ++i) {
        input(static_cast<TSelectorInput::Number>(i))->setDisabled(false);
        input(static_cast<TSelectorInput::Number>(i))->link()->setVirtual(false);
    }

    for (uint i = _lastActiveInput + 1; i <= TSelectorInput::N_4; ++i) {
        input(static_cast<TSelectorInput::Number>(i))->setDisabled(true);
        input(static_cast<TSelectorInput::Number>(i))->link()->setVirtual(true);
        input(static_cast<TSelectorInput::Number>(i))->faultLink()->setVirtual(true);
    }
}

TRITONN_END_NAMESPACE
