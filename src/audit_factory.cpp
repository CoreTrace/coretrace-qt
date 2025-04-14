#include "../includes/audit_factory.hpp"

AuditService* AuditFactory::createAuditService() {
    return new AuditService();
} 