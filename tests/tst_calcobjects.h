#ifndef TSTCALCOBJECTS_H
#define TSTCALCOBJECTS_H

#include "defs.h"

class TstCalcObjects: public QObject {
    Q_OBJECT

public:
    TstCalcObjects();
    ~TstCalcObjects();

private slots:
    void test_selector_properties_data();
    void test_selector_properties();

    void test_selector_setup();
    void test_selector_mode();

private:
    TSelector *_selector = nullptr;
};

#endif // TSTCALCOBJECTS_H
