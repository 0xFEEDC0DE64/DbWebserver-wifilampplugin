#pragma once

#include "webapplication.h"

class QJsonObject;

class WifiLampApplication : public WebApplication
{
    Q_OBJECT

public:
    WifiLampApplication(const QJsonObject &config, QObject *parent = Q_NULLPTR);

    void start() Q_DECL_OVERRIDE;
};
