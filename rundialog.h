#ifndef RUNDIALOG_H
#define RUNDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include "debugconfiguration.h"

class RunDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RunDialog(QWidget *parent = nullptr);
    
    void setConfigurations(const QList<DebugConfiguration> &configs);
    DebugConfiguration selectedConfiguration() const;
    QString selectedMode() const; // "run" or "debug"

private slots:
    void onRunClicked();
    void onDebugClicked();
    void onEditConfigurations();

private:
    QComboBox *m_configCombo;
    QPushButton *m_runButton;
    QPushButton *m_debugButton;
    QPushButton *m_editButton;
    QPushButton *m_cancelButton;
    
    QString m_selectedMode;
};

#endif // RUNDIALOG_H
