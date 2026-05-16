#pragma once

template<typename... Args>
void sendUpdate(const char *update, bool onlyLog, Args... args);
