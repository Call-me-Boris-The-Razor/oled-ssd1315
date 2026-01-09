/**
 * @file MockI2c.hpp
 * @brief Mock-реализация II2c для unit-тестирования
 */

#ifndef OLED_MOCK_I2C_HPP
#define OLED_MOCK_I2C_HPP

#include "../../include/oled/ports/II2c.hpp"
#include <vector>
#include <cstring>

namespace oled {
namespace test {

/**
 * @brief Mock I2C адаптер для тестирования
 *
 * Записывает все отправленные данные и позволяет
 * симулировать ответы устройства.
 */
class MockI2c : public II2c {
public:
    struct Transaction {
        uint8_t addr7;
        std::vector<uint8_t> data;
    };

    MockI2c() = default;

    // === II2c interface ===

    bool write(uint8_t addr7, const uint8_t* data, size_t len) override {
        if (shouldFail_) {
            return false;
        }

        Transaction tx;
        tx.addr7 = addr7;
        tx.data.assign(data, data + len);
        transactions_.push_back(tx);

        return true;
    }

    bool probe(uint8_t addr7) override {
        if (shouldFail_) {
            return false;
        }

        for (uint8_t addr : respondingAddresses_) {
            if (addr == addr7) {
                return true;
            }
        }
        return false;
    }

    // === Test helpers ===

    /**
     * @brief Получить все записанные транзакции
     */
    const std::vector<Transaction>& transactions() const {
        return transactions_;
    }

    /**
     * @brief Очистить записанные транзакции
     */
    void clearTransactions() {
        transactions_.clear();
    }

    /**
     * @brief Получить количество транзакций
     */
    size_t transactionCount() const {
        return transactions_.size();
    }

    /**
     * @brief Симулировать сбой I2C
     */
    void setFail(bool fail) {
        shouldFail_ = fail;
    }

    /**
     * @brief Добавить адрес, который будет отвечать на probe()
     */
    void addRespondingAddress(uint8_t addr7) {
        respondingAddresses_.push_back(addr7);
    }

    /**
     * @brief Очистить список отвечающих адресов
     */
    void clearRespondingAddresses() {
        respondingAddresses_.clear();
    }

    /**
     * @brief Получить последнюю транзакцию
     */
    const Transaction* lastTransaction() const {
        if (transactions_.empty()) {
            return nullptr;
        }
        return &transactions_.back();
    }

    /**
     * @brief Проверить, содержит ли последняя транзакция указанные данные
     */
    bool lastTransactionContains(const uint8_t* expected, size_t len) const {
        if (transactions_.empty()) {
            return false;
        }
        const auto& last = transactions_.back();
        if (last.data.size() < len) {
            return false;
        }
        return memcmp(last.data.data(), expected, len) == 0;
    }

private:
    std::vector<Transaction> transactions_;
    std::vector<uint8_t> respondingAddresses_;
    bool shouldFail_ = false;
};

} // namespace test
} // namespace oled

#endif // OLED_MOCK_I2C_HPP
