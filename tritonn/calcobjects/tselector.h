#ifndef TSELECTOR_H
#define TSELECTOR_H

#include "tcalcobjectabstract.h"
#include "../tinputvalue.h"
#include "../toutputvalue.h"
#include "../tinternalvalue.h"

TRITONN_BEGIN_NAMESPACE

class TRITONN_LIBRARY TSelectorInput : public TInputValue {
public:
    enum Number : uint {
        N_1 = 1,
        N_2 = 2,
        N_3 = 3,
        N_4 = 4
    };

    TSelectorInput(const QString &opcTagName, Unit unit, TObject *parent = nullptr);
    virtual ~TSelectorInput() override = default;

    virtual void validate(TValidateEvent &event) const override;

    inline TLink *faultLink() const  { return _faultLink; }
    inline TUInt32 *fault() const { return _fault; }

    void setUnitUpdater(TUnitUpdater *updater);

protected:
    TLink *_faultLink = nullptr;
    TUInt32 *_fault = nullptr;
};


class TRITONN_LIBRARY TSelector : public TCalcObjectAbstract {
public:
    enum Mode {
        M_NothingToDo     = 1,  //Не переходить в случае аварии на другое входное значение
        M_SelectKeypad    = 2,  //В случае аварии переходить на ручной ввод (keypad)
        M_SelectNextInput = 3,  //В случае аварии переходить на предыдущее входное значение
        M_SelectPrevInput = 4   //В случае аварии переходить на следующее входное значение
    };

    enum SetupFlag : uint16_t {
        SF_Off     = 0x0001,    //Селектор отключен
        SF_NoEvent = 0x0004,    //Не выдавать сообщений
        SF_Multi   = 0x8000     //Флаг мультиселектора (внутренний), не указывать в документации
    };

    explicit TSelector(TCalcObjects *parent = nullptr);
    explicit TSelector(const QString &opcTagName, TCalcObjects *parent = nullptr);

    void validate(TValidateEvent &event) const override;

    bool hasActivedFaultLinks() const;

    void activateFaultLinks();
    void deactivateFaultLinks();

    TSelector::Mode getBehaviorOnFail() const;
    void setBehaviorOnFail(TSelector::Mode behavior);

    bool isSuppressedEventMessages() const;
    void setSuppressEventMessages(bool supress);

    TSelectorInput *input(TSelectorInput::Number number) const;

    inline TSelectorInput::Number getLastActiveInput() const { return _lastActiveInput; }
    void setLastActiveInput(TSelectorInput::Number number);

    inline TSelectorInput::Number getInitialInput() const { return _initialInput; }
    inline void setInitialInput(TSelectorInput::Number number) { _initialInput = number; }

    Unit getUnit() const;
    inline UnitType getUnitType() const { return toUnitType(getUnit()); }
    void setUnit(Unit unit);

    int getCountLinkedInputs() const;
    void clearLinkedInputs();

    inline TInt16  *select()     const { return _select;     }
    inline TUInt16 *inputCount() const { return _inputCount; }
    inline TUInt16 *mode()       const { return _mode;       }
    inline TUInt16 *setup()      const { return _setup;      }
    inline TUInt32 *fault()      const { return _fault;      }

    inline TOutputValue   *output() const { return _output; }
    inline TInternalValue *keypad() const { return _keypad; }

    static QString getXmlTagName() { return "selector"; }
    void serialize(TXmlElementRef xml_parent, const TXmlVersion &xml_version) const override;
    void deserialize(const TXmlElementRef xml_this, const TXmlVersion &xml_version) override;

protected:
    inline void appendInput(TSelectorInput *input) { appendNode(input); }
    inline void removeInput(TSelectorInput *input) { removeNode(input); }

    void updateEnabledInputs();

private:
    void setUnitUpdater();

private:
    TSelectorInput::Number _lastActiveInput = TSelectorInput::N_4;
    TSelectorInput::Number _initialInput    = TSelectorInput::N_1;

    TInt16 *_select      = nullptr;
    TUInt16 *_inputCount = nullptr;
    TUInt16 *_mode       = nullptr;
    TUInt16 *_setup      = nullptr;
    TUInt32 *_fault      = nullptr;

    TOutputValue   *_output = nullptr;
    TInternalValue *_keypad = nullptr;

    std::unique_ptr<TUnitUpdater> _unitsUpdater;
};

TRITONN_END_NAMESPACE

#endif // TSELECTOR_H
