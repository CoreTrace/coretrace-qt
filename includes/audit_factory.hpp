#pragma once

#include "audit_service.hpp"

class AuditFactory {
public:
    static AuditService* createAuditService();
}; 