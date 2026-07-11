#ifndef RUNDIALOG_H
#define RUNDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include "debugconfiguration.h"

class RunDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RunDialog(QWidget *parent = nullptr);

    void setConfigurations(const QList<DebugConfiguration> &configs);
    QList<DebugConfiguration> configurations() const { return m_configs; }
    DebugConfiguration selectedConfiguration() const;
    QString selectedMode() const; // "run" or "debug"

private slots:
    void onRunClicked();
    void onDebugClicked();
    void onAddClicked();
    void onDeleteClicked();
    void onEditClicked();
    void onApplyClicked();
    void onSelectionChanged(int index);

private:
    void loadConfigIntoForm(const DebugConfiguration &config);

    QComboBox *m_configCombo;
    QPushButton *m_runButton;
    QPushButton *m_debugButton;
    QPushButton *m_addButton;
    QPushButton *m_deleteButton;
    QPushButton *m_editButton;
    QPushButton *m_applyButton;
    QPushButton *m_cancelButton;

    QLineEdit *m_nameEdit;
    QLineEdit *m_programEdit;
    QLineEdit *m_argsEdit;
    QLineEdit *m_cwdEdit;
    QLineEdit *m_debuggerEdit;

    QList<DebugConfiguration> m_configs;
    QString m_selectedMode;
};

#endif // RUNDIALOG_H
