#include "../includes/audit_factory.hpp"

/**
 * @brief Creates and returns a new instance of AuditService.
 * 
 * This method is responsible for dynamically allocating and returning
 * a new AuditService object. The caller is responsible for managing
 * the lifetime of the returned object and ensuring it is properly
 * deleted to avoid memory leaks.
 * 
 * @return A pointer to a newly created AuditService instance.
 */
AuditService* AuditFactory::createAuditService() {
    return new AuditService();
} 