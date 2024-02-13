#include <iostream>
#include <vector>
#include <cstring>


size_t get_number_len(size_t number) {
    size_t size = 0;
    for(; number != 0; number /= 10) {
        size++;
    }

    return size;
}


class BigInt {
    friend std::ostream& operator<<(std::ostream& output, const BigInt& number);

    size_t _size;
    uint16_t* _digits;

    void _create_digits(size_t size) {
        _digits = new uint16_t[size]();
    }

    void _delete_digits() {
        delete[] _digits;
    }

    uint16_t* _copy_digits() {
        auto copy_digits = new uint16_t[_size];
        std::copy(_digits, _digits + _size, copy_digits);
        return copy_digits;
    }

    void _copy_to_digits(uint16_t* other_digits, size_t size) {
        std::copy(other_digits, other_digits + size, _digits);
    }

public:
    BigInt(uint16_t* digits, size_t size) { // NOLINT(cppcoreguidelines-pro-type-member-init)
        /* Create from raw */

        _size = size;
        _create_digits(size);
        _copy_to_digits(digits, size);
    }

    explicit BigInt(const char* string) { // NOLINT(cppcoreguidelines-pro-type-member-init)
        _size = strlen(string);

        _create_digits(_size);

        for(size_t i = 0; i < _size; i++) {
            _digits[i] = string[(_size-1) - i] - '0';
        }

    }

    explicit BigInt(size_t number) { // NOLINT(cppcoreguidelines-pro-type-member-init)
        _size = get_number_len(number);

        _create_digits(_size);

        for(size_t i = 0; number != 0; number /= 10, i++) {
            uint16_t digit = number % 10;

            _digits[i] = digit;
        }
    }

    BigInt(const BigInt& other) { // NOLINT(cppcoreguidelines-pro-type-member-init)
        _size = other._size;

        _create_digits(_size);
        _copy_to_digits(other._digits, _size);
    }

    ~BigInt() {
        _delete_digits();
    }

    BigInt& operator=(const BigInt& other) {
        if(this == &other) {
            return *this;
        }

        _size = other._size;

        _delete_digits();
        _create_digits(_size);
        _copy_to_digits(other._digits, _size);

        return *this;
    }

    BigInt& operator+=(const BigInt& other) {
        size_t new_size = std::max(_size, other._size) + 1;  // 9x + 9x = 1xx

        auto* copy_digits = _copy_digits();
        _delete_digits();
        _create_digits(new_size);
        _digits = copy_digits;

        size_t i = 0;
        uint16_t new_digit = 0;
        for(; i < other._size; i++) {
            new_digit += _digits[i] + other._digits[i];
            _digits[i] = new_digit % 10;
            new_digit = new_digit / 10;
        }
        for(; i < _size; i++) {
            new_digit += _digits[i];
            _digits[i] = new_digit % 10;
            new_digit = new_digit / 10;
        }

        _size = new_size;
        if (new_digit == 0) {
            _size--;
        } else {
            _digits[i] = new_digit;
        }

        return *this;
    }

    BigInt& operator*=(const BigInt& other) {
        size_t new_size = _size + other._size;  // 9x * 9x = 9xxx
        auto* copy_digits = _copy_digits();
        _delete_digits();
        _create_digits(new_size);
        _digits = copy_digits;

        std::vector<BigInt> numbers_for_addition(other._size);

        for(size_t i = 0; i < other._size; i++) {
            auto& second_multiplier = other._digits[i];
            auto& number_for_addition = numbers_for_addition[i];
            number_for_addition._create_digits(new_size);
            number_for_addition._size = new_size;

            uint16_t new_digit = 0;
            size_t j;
            for(j = 0; j < i; j++) {
                number_for_addition._digits[j] = 0;
            }
            for(j = 0; j < _size; j++) {
                new_digit += _digits[j] * second_multiplier;

                number_for_addition._digits[j+i] = new_digit % 10;
                new_digit = new_digit / 10;
            }
            number_for_addition._digits[j+i] = new_digit;
        }

        _size = new_size;
        auto sum_ = numbers_for_addition[0];

        for(size_t i = 1; i < other._size; i++) {
            sum_ += numbers_for_addition[i];
        }

        *this = BigInt(sum_);

        return *this;
    }

    BigInt operator+(const BigInt& other) {
        BigInt copy(*this);
        copy += other;
        return copy;
    }

    BigInt operator*(const BigInt& other) {
        BigInt copy(*this);
        copy *= other;
        return copy;
    }

    bool operator==(const BigInt& other) {
        if (_size != other._size) {
            return false;
        }

        for(size_t i = _size-1; i != -1; i--) {
            if (_digits[i] != other._digits[i]) {
                return false;
            }
        }
        return true;
    }
};

std::ostream& operator<<(std::ostream& output, const BigInt& number) {
    for(size_t i = number._size-1; i != -1; i--) {
        output << (size_t)number._digits[i];
    }
    return output;
}


int main() {
    BigInt a("151353562466743631513535624667436315135356246674363151353562466743631513535624667436315135356246674363");
    BigInt b("151353562466743631513535624667436315135356246674363151353562466743631513535624667436315135356246674363");

    //a *= b;
    //std::cout << a << std::endl;
    //std::cout << BigInt("5") + BigInt("50") << std::endl;
    std::cout << (BigInt(5) *= BigInt(1000)) << std::endl;
    std::cout << (BigInt(5) *= BigInt(1000)) << std::endl;
    std::cout << (BigInt(5) *= BigInt(1000)) << std::endl;
    std::cout << (BigInt(5) *= BigInt(1000)) << std::endl;
    std::cout << (BigInt(5) *= BigInt(1000)) << std::endl;

    return 0;
}
