#ifndef TSELECTORWIDGET_H
#define TSELECTORWIDGET_H

#include "twidget.h"

namespace Ui {
class TSelectorWidget;
}

class TSelectorWidget : public TWidget {
    Q_OBJECT

public:
    explicit TSelectorWidget(TTritonn *tritonn, QWidget *parent = nullptr);
    explicit TSelectorWidget(TSelector *object, TTritonn *tritonn, QWidget *parent = nullptr);
    ~TSelectorWidget();

    void setCalcObject(TSelector *object);

signals:
    void linkNameChanged();
    void linkActivated(TObject *object);
    void translateActivated(uint32_t code, const QString &lang);
    void formattingActivated(Unit unit);

private slots:
    void on_pushButton_inputLinkSelect_1_released();
    void on_pushButton_inputLinkSelect_2_released();
    void on_pushButton_inputLinkSelect_3_released();
    void on_pushButton_inputLinkSelect_4_released();

    void on_pushButton_faultLinkSelect_1_released();
    void on_pushButton_faultLinkSelect_2_released();
    void on_pushButton_faultLinkSelect_3_released();
    void on_pushButton_faultLinkSelect_4_released();

    void on_pushButton_translate_released();

    void on_pushButton_autobindFaultLink_1_released();
    void on_pushButton_autobindFaultLink_2_released();
    void on_pushButton_autobindFaultLink_3_released();
    void on_pushButton_autobindFaultLink_4_released();

    void on_pushButton_formatting_released();

private:
    void setGeneral();
    void setDescription();
    void setInputs();
    void setFaults();

    void setValidators();
    void setValidatorName();
    void setValidatorValue();

    void connectSignals();
    void connectGeneralSignals();
    void connectDescriptionSignals();
    void connectInputSignals();
    void connectFaultSignals();

    void selectInputLink(TSelectorInput::Number number);
    void selectFaultLink(TSelectorInput::Number number);

    void updateRepresentation();
    void updateComboBoxUnits(UnitType type);

    void updateInitialInput();

    void updateInputs();

    void updateInput_1();
    void updateInput_2();
    void updateInput_3();
    void updateInput_4();

    void updateFaults();

    void updateFault_1();
    void updateFault_2();
    void updateFault_3();
    void updateFault_4();

private:
    Ui::TSelectorWidget *ui = nullptr;
    TSelector *_calcObject = nullptr;
    QDoubleValidator *_valueValidator = new QDoubleValidator(0, 1000000000000, 2, this);
};

#endif // TSELECTORWIDGET_H
