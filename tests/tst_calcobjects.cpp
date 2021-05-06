#include "tst_calcobjects.h"

TstCalcObjects::TstCalcObjects() {
    _selector = new TSelector();
}

TstCalcObjects::~TstCalcObjects() {
    delete _selector; _selector = nullptr;
}

void TstCalcObjects::test_selector_properties_data() {
    QTest::addColumn<uint32_t>("direction");
    QTest::addColumn<uint32_t>("type");
    QTest::addColumn<uint32_t>("unitId");
    QTest::addColumn<bool>    ("readonly");
    QTest::addColumn<uint32_t>("result_direction");
    QTest::addColumn<uint32_t>("result_type");
    QTest::addColumn<uint32_t>("result_unitId");
    QTest::addColumn<bool>    ("result_readonly");

    TDataAbstract *data = nullptr;

    data = _selector->input(TSelectorInput::N_1)->value();
    QTest::newRow("selector.input_1.value") << uint32_t(data->getDirection())
                                            << uint32_t(data->getType())
                                            << uint32_t(data->getUnit()) << data->isReadOnly()
                                            << IN << LREAL << 0u << Yes;

    data = _selector->input(TSelectorInput::N_1)->unit();
    QTest::newRow("selector.input_1.unit") << uint32_t(data->getDirection())
                                           << uint32_t(data->getType())
                                           << uint32_t(data->getUnit()) << data->isReadOnly()
                                           << WO << STRID << 512u << Yes;

    data = _selector->input(TSelectorInput::N_1)->fault();
    QTest::newRow("selector.input_1.fault") << uint32_t(data->getDirection())
                                            << uint32_t(data->getType())
                                            << uint32_t(data->getUnit()) << data->isReadOnly()
                                            << IN << UDINT << 512u << Yes;

    data = _selector->input(TSelectorInput::N_2)->value();
    QTest::newRow("selector.input_2.value") << uint32_t(data->getDirection())
                                            << uint32_t(data->getType())
                                            << uint32_t(data->getUnit()) << data->isReadOnly()
                                            << IN << LREAL << 0u << Yes;

    data = _selector->input(TSelectorInput::N_2)->unit();
    QTest::newRow("selector.input_2.unit") << uint32_t(data->getDirection())
                                           << uint32_t(data->getType())
                                           << uint32_t(data->getUnit()) << data->isReadOnly()
                                           << WO << STRID << 512u << Yes;

    data = _selector->input(TSelectorInput::N_2)->fault();
    QTest::newRow("selector.input_2.fault") << uint32_t(data->getDirection())
                                            << uint32_t(data->getType())
                                            << uint32_t(data->getUnit()) << data->isReadOnly()
                                            << IN << UDINT << 512u << Yes;

    data = _selector->input(TSelectorInput::N_3)->value();
    QTest::newRow("selector.input_3.value") << uint32_t(data->getDirection())
                                            << uint32_t(data->getType())
                                            << uint32_t(data->getUnit()) << data->isReadOnly()
                                            << IN << LREAL << 0u << Yes;

    data = _selector->input(TSelectorInput::N_3)->unit();
    QTest::newRow("selector.input_3.unit") << uint32_t(data->getDirection())
                                           << uint32_t(data->getType())
                                           << uint32_t(data->getUnit()) << data->isReadOnly()
                                           << WO << STRID << 512u << Yes;

    data = _selector->input(TSelectorInput::N_3)->fault();
    QTest::newRow("selector.input_3.fault") << uint32_t(data->getDirection())
                                            << uint32_t(data->getType())
                                            << uint32_t(data->getUnit()) << data->isReadOnly()
                                            << IN << UDINT << 512u << Yes;

    data = _selector->input(TSelectorInput::N_4)->value();
    QTest::newRow("selector.input_4.value") << uint32_t(data->getDirection())
                                            << uint32_t(data->getType())
                                            << uint32_t(data->getUnit()) << data->isReadOnly()
                                            << IN << LREAL << 0u << Yes;

    data = _selector->input(TSelectorInput::N_4)->unit();
    QTest::newRow("selector.input_4.unit") << uint32_t(data->getDirection())
                                           << uint32_t(data->getType())
                                           << uint32_t(data->getUnit()) << data->isReadOnly()
                                           << WO << STRID << 512u << Yes;

    data = _selector->input(TSelectorInput::N_4)->fault();
    QTest::newRow("selector.input_4.fault") << uint32_t(data->getDirection())
                                            << uint32_t(data->getType())
                                            << uint32_t(data->getUnit()) << data->isReadOnly()
                                            << IN << UDINT << 512u << Yes;

    data = _selector->output()->value();
    QTest::newRow("selector.output.unit") << uint32_t(data->getDirection())
                                          << uint32_t(data->getType())
                                          << uint32_t(data->getUnit()) << data->isReadOnly()
                                          << OUT << LREAL << 0u << Yes;

    data = _selector->output()->unit();
    QTest::newRow("selector.output.unit") << uint32_t(data->getDirection())
                                          << uint32_t(data->getType())
                                          << uint32_t(data->getUnit()) << data->isReadOnly()
                                          << WO << STRID << 512u << Yes;

    data = _selector->select();
    QTest::newRow("selector.select") << uint32_t(data->getDirection())
                                     << uint32_t(data->getType())
                                     << uint32_t(data->getUnit()) << data->isReadOnly()
                                     << WO << INT << 512u << No;

    data = _selector->inputCount();
    QTest::newRow("selector.inputCount") << uint32_t(data->getDirection())
                                         << uint32_t(data->getType())
                                         << uint32_t(data->getUnit()) << data->isReadOnly()
                                         << WO << UINT << 512u << Yes;

    data = _selector->setup();
    QTest::newRow("selector.setup") << uint32_t(data->getDirection())
                                    << uint32_t(data->getType())
                                    << uint32_t(data->getUnit()) << data->isReadOnly()
                                    << WO << UINT << 512u << Yes;

    data = _selector->mode();
    QTest::newRow("selector.mode") << uint32_t(data->getDirection())
                                   << uint32_t(data->getType())
                                   << uint32_t(data->getUnit()) << data->isReadOnly()
                                   << WO << UINT << 512u << No;

    data = _selector->fault();
    QTest::newRow("selector.fault") << uint32_t(data->getDirection())
                                    << uint32_t(data->getType())
                                    << uint32_t(data->getUnit()) << data->isReadOnly()
                                    << WO << UDINT << 512u << Yes;

    data = _selector->keypad()->unit();
    QTest::newRow("selector.keypad.unit") << uint32_t(data->getDirection())
                                          << uint32_t(data->getType())
                                          << uint32_t(data->getUnit()) << data->isReadOnly()
                                          << WO << UDINT << 512u << Yes;

    data = _selector->keypad()->value();
    QTest::newRow("selector.keypad.value") << uint32_t(data->getDirection())
                                           << uint32_t(data->getType())
                                           << uint32_t(data->getUnit()) << data->isReadOnly()
                                           << WO << LREAL << 0u << No;
}

void TstCalcObjects::test_selector_properties() {
    QFETCH(uint32_t, direction);
    QFETCH(uint32_t, type);
    QFETCH(uint32_t, unitId);
    QFETCH(bool,     readonly);
    QFETCH(uint32_t, result_direction);
    QFETCH(uint32_t, result_type);
    QFETCH(uint32_t, result_unitId);
    QFETCH(bool,     result_readonly);

    QCOMPARE(direction, result_direction);
    QCOMPARE(type,      result_type);
    QCOMPARE(unitId,    result_unitId);
    QCOMPARE(readonly,  result_readonly);
}

void TstCalcObjects::test_selector_setup() {
    _selector->setSuppressEventMessages(true);
    QVERIFY(_selector->setup()->testFlag(TSelector::SF_NoEvent));
}

void TstCalcObjects::test_selector_mode() {
    _selector->setBehaviorOnFail(TSelector::M_NothingToDo);
    QCOMPARE(_selector->mode()->getData<TSelector::Mode>(), TSelector::M_NothingToDo);

    _selector->setBehaviorOnFail(TSelector::M_SelectKeypad);
    QCOMPARE(_selector->mode()->getData<TSelector::Mode>(), TSelector::M_SelectKeypad);

    _selector->setBehaviorOnFail(TSelector::M_SelectNextInput);
    QCOMPARE(_selector->mode()->getData<TSelector::Mode>(), TSelector::M_SelectNextInput);

    _selector->setBehaviorOnFail(TSelector::M_SelectPrevInput);
    QCOMPARE(_selector->mode()->getData<TSelector::Mode>(), TSelector::M_SelectPrevInput);
}
