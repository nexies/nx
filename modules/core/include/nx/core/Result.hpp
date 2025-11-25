//
// Created by nexie on 08.11.2025.
//

#ifndef RESULT_HPP
#define RESULT_HPP

#include <type_traits>
#include <variant>
#include <optional>
#include <utility>
#include <stdexcept>

namespace nx
{
    namespace detail
    {
        enum class result_type_e
        {
            ok_type,
            err_type,
        };

        template <typename OkType, typename ErrType, typename Enable = void>
        struct result_storage_t;

        template <typename Type, typename Enable>
        struct result_storage_t<Type, Type, Enable>
        {
            Type _d;

            result_storage_t(Type data) : _d(std::move(data)) {}
            Type get_ok() const { return _d; }
            Type get_err() const { return _d; }
        };

        template <typename ErrType>
        struct result_storage_t<void, ErrType>
        {
            std::optional<ErrType> _d;

            result_storage_t() : _d(std::nullopt) {}
            result_storage_t(ErrType err) : _d(std::move(err)) {}
            ErrType get_err() const { return _d.value(); }
        };

        template <typename OkType, typename ErrType>
        struct result_storage_t<OkType, ErrType, std::enable_if_t<!std::is_same<OkType, void>::value && !std::is_same<
                                    OkType, ErrType>::value>>
        {
            std::variant<OkType, ErrType> _d;

            result_storage_t(OkType ok) : _d(std::move(ok)) {}
            result_storage_t(ErrType err) : _d(std::move(err)) {}
            OkType get_ok() const { return std::get<OkType>(_d); }
            ErrType get_err() const { return std::get<ErrType>(_d); }
        };
    } // namespace detail

    struct result_exception : public std::runtime_error
    {
        result_exception(const char * p_what = "result exception") :
            std::runtime_error(p_what)
        {}
    };

    template <typename OkType, typename ErrType, typename Enable = void>
    class result_t;

    template <typename ErrType>
    class result_t<void, ErrType>
    {
        static_assert(!std::is_same<ErrType, void>::value, "Error type cannot be void");
        detail::result_storage_t<void, ErrType> _storage;
        detail::result_type_e _type;

        result_t() : _storage(), _type(detail::result_type_e::ok_type) { }
        template <typename... Args>
        result_t(Args&&... args) : _storage(std::forward<Args>(args)...), _type(detail::result_type_e::err_type) { }

    public:
        static result_t Ok() { return result_t(); }
        static result_t Err(ErrType err = ErrType()) { return result_t(err); }

        ErrType get_err() const { if (_type != detail::result_type_e::err_type) throw result_exception("result is not err"); return _storage.get_err(); }
        explicit operator bool() const { return _type == detail::result_type_e::ok_type; }

        bool operator ==(const result_t& other) const
        {
            if (_type == other._type)
                if (_type == detail::result_type_e::err_type)
                { return get_err() == other.get_err(); }
                else
                { return true; }
            return false;
        }
        bool operator !=(const result_t& other) const
        {
            return !this->operator ==(other);
        }
    };

    template <typename OkType, typename ErrType>
    class result_t<OkType, ErrType>
    {
        static_assert(!std::is_same<ErrType, void>::value, "Error type cannot be void");
        detail::result_storage_t<OkType, ErrType> _storage;
        detail::result_type_e _type;

        template <typename... Args>
        explicit result_t(detail::result_type_e type, Args&&... args) : _storage(std::forward<Args>(args)...), _type(type) { }

    public:
        static result_t Ok(OkType && ok = OkType()) { return result_t(detail::result_type_e::ok_type, std::move(ok)); }
        static result_t Err(ErrType && err = ErrType()) { return result_t(detail::result_type_e::err_type, std::move(err)); }

        OkType get_ok() const { if (_type != detail::result_type_e::ok_type) throw result_exception("result is not ok");  return _storage.get_ok(); }
        ErrType get_err() const { if (_type != detail::result_type_e::err_type) throw result_exception("result is not err"); return _storage.get_err(); }

        explicit operator bool() const { return _type == detail::result_type_e::ok_type; }
        bool operator ==(const result_t& other) const
        {
            if (_type == other._type)
                if (_type == detail::result_type_e::err_type)
                { return get_err() == other.get_err(); }
                else
                { return get_ok() == other.get_ok(); }
            return false;
        }
        bool operator !=(const result_t& other) const
        {
            return !this->operator ==(other);
        }
    };

    namespace common {
        struct common_result_t {
        protected:
            const char * _str;
            int32_t _code;
        public:
            common_result_t(int32_t code, const char * str) : _str(str), _code(code) {};
            int32_t code () { return _code; }
            const char * str () { return _str; }
        };

        struct common_ok_t : public common_result_t {
            common_ok_t(const char * str = "Success") : common_result_t(0, str) {};
            common_ok_t(int32_t code) : common_result_t(code, "Success") {};
            common_ok_t(int32_t code, const char * str) : common_result_t(code, str) {}
        };

        struct common_err_t : public common_result_t {
            common_err_t(const char * str) : common_result_t(-1, str) {};
            common_err_t(int32_t code) : common_result_t(code, "Error") {};
            common_err_t(int32_t code, const char * str) : common_result_t(code, str) {}
        };
    }

    using Result = result_t<common::common_ok_t, common::common_err_t>;

} // namespace nx

#endif //RESULT_HPP
