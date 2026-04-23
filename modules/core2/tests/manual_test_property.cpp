//
// Created by nexie on 4/20/2026.
//

#include <iostream>
#include <nx/core2/object/meta_property.hpp>
#include <nx/core2/object/object.hpp>
#include <nx/core2/detail/property_defs2.hpp>

struct obj : public nx::core::object
{
    NX_OBJECT(obj)

    int value = 0;

    int get_value() const { return value; }

    void set_value (const int & value) { this->value = value; }



    _nxpv2_entry(first_name, TYPE std::string, READ, WRITE, NOTIFY name_changed)

    NX_SIGNAL(name_changed, std::string)

};

int main ()
{
    auto meta = obj::static_meta_object();
    auto req = meta.property_registry();

    // std::cerr << NX_TO_STRING(_nxpv2_sort_args_d(0, TYPE int)) << std::endl;
    // std::cerr << NX_TO_STRING(_nxpv2_entry(value, TYPE int)) << std::endl << std::endl;
    //
    // std::cerr << NX_TO_STRING(_nxpv2_tokenize(READ)) << std::endl;
    // // std::cerr << NX_TO_STRING(_nxpv2_entry(value, TYPE int, READ)) << std::endl << std::endl;
    //
    // // std::cerr << NX_TO_STRING(_nxpv2_sort_args_d(0, TYPE int, READ)) << std::endl << std::endl;
    // std::cerr << NX_TO_STRING(_nxpv2_sort_args_cond_d(0, (), TYPE int, READ)) << std::endl << std::endl;
    // std::cerr << NX_TO_STRING(_nxpv2_sort_args_res_d(0, (), TYPE int, READ)) << std::endl << std::endl;
    //
    // // std::cerr << NX_TO_STRING(_nxpv2_sort_args_op_d(0, (), TYPE int, READ)) << std::endl << std::endl;
    // std::cerr << NX_TO_STRING((_nxpv2_args_add(_nxpv2_default_args, _nxpv2_tokenize(TYPE int)), READ)) << std::endl << std::endl;
    // std::cerr << NX_TO_STRING((_nxpv2_args_add(_nxpv2_default_args, _nxpv2_tokenize(TYPE int)) _nx_append_args(READ))) << std::endl << std::endl;
    // std::cerr << NX_TO_STRING((_nxpv2_args_add(_nxpv2_default_args, _nxpv2_tokenize(READ)) _nx_append_args())) << std::endl << std::endl;
    // std::cerr << NX_TO_STRING((_nxpv2_tokenize(READ))) << std::endl << std::endl;
    // // std::cerr << NX_TO_STRING(_nxpv2_args_add(((1, (int))), (2))) << std::endl << std::endl;
    //
    // std::cerr << NX_TO_STRING((1, ( _nx_args_token_value((2)) ))) << std::endl << std::endl;

    return 0;



}