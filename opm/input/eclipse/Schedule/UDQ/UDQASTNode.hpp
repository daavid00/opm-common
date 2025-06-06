/*
  Copyright 2019 Equinor ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef UDQASTNODE_HPP
#define UDQASTNODE_HPP

#include <opm/input/eclipse/Schedule/UDQ/UDQContext.hpp>
#include <opm/input/eclipse/Schedule/UDQ/UDQEnums.hpp>
#include <opm/input/eclipse/Schedule/UDQ/UDQSet.hpp>

#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

namespace Opm {

class UDQASTNode
{
public:
    UDQVarType var_type { UDQVarType::NONE };

    UDQASTNode();
    explicit UDQASTNode(UDQTokenType type_arg);
    explicit UDQASTNode(double scalar_value);

    UDQASTNode(UDQTokenType type_arg,
               const std::variant<std::string, double>& value_arg,
               const UDQASTNode& left_arg);

    UDQASTNode(UDQTokenType type_arg,
               const std::variant<std::string, double>& value_arg,
               const UDQASTNode& left,
               const UDQASTNode& right);

    UDQASTNode(UDQTokenType type_arg,
               const std::variant<std::string, double>& value_arg);

    UDQASTNode(UDQTokenType type_arg,
               const std::variant<std::string, double>& value_arg,
               const std::vector<std::string>& selector);

    static UDQASTNode serializationTestObject();

    UDQSet eval(UDQVarType eval_target, const UDQContext& context) const;
    bool valid() const;
    std::set<UDQTokenType> func_tokens() const;

    void update_type(const UDQASTNode& arg);
    void set_left(const UDQASTNode& arg);
    void set_right(const UDQASTNode& arg);
    void scale(double sign_factor);

    UDQASTNode* get_left() const;
    UDQASTNode* get_right() const;
    bool operator==(const UDQASTNode& data) const;
    void required_summary(std::unordered_set<std::string>& summary_keys) const;

    /// Populate collection of requisite objects needed to evaluate this node.
    ///
    /// \param[in,out] objects Specific Schedule objects named in containing
    /// UDQ definition.  On exit also contains the specific objects needed
    /// by this node and, recursively, its children.
    void requiredObjects(UDQ::RequisiteEvaluationObjects& objects) const;

    template <class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(var_type);
        serializer(type);
        serializer(value);
        serializer(sign);
        serializer(selector);
        serializer(left);
        serializer(right);
    }

private:
    UDQTokenType type;

    std::variant<std::string, double> value;
    double sign = 1.0;
    std::vector<std::string> selector;
    std::shared_ptr<UDQASTNode> left;
    std::shared_ptr<UDQASTNode> right;

    UDQSet eval_expression(const UDQContext& context) const;

    UDQSet eval_well_expression(const std::string& string_value,
                                const UDQContext&  context) const;

    UDQSet eval_group_expression(const std::string& string_value,
                                 const UDQContext&  context) const;

    UDQSet eval_segment_expression(const std::string& string_value,
                                   const UDQContext&  context) const;

    UDQSet eval_region_expression(const std::string& string_value,
                                  const UDQContext&  context) const;

    UDQSet eval_scalar_function(const UDQVarType  target_type,
                                const UDQContext& context) const;

    UDQSet eval_elemental_unary_function(const UDQVarType  target_type,
                                         const UDQContext& context) const;

    UDQSet eval_binary_function(const UDQVarType  target_type,
                                const UDQContext& context) const;

    UDQSet eval_number(const UDQVarType  target_type,
                       const UDQContext& context) const;

    UDQSet eval_table_lookup(const UDQVarType target_type,
                             const std::string& string_value,
                             const UDQContext& context) const;

    UDQSet eval_table_lookup_field(const std::string& string_value,
                                   const UDQContext& context) const;
    UDQSet eval_table_lookup_group(const std::string& string_value,
                                   const UDQContext& context) const;
    UDQSet eval_table_lookup_segment(const std::string& string_value,
                                     const UDQContext& context) const;
    UDQSet eval_table_lookup_well(const std::string& string_value,
                                  const UDQContext& context) const;

    void func_tokens(std::set<UDQTokenType>& tokens) const;

    /// Populate collection of requisite objects needed to evaluate this
    /// node.
    ///
    /// \param[in,out] objects Specific Schedule objects named in containing
    /// UDQ definition.  On exit also contains the specific objects needed
    /// by this node.
    void populateRequiredObjects(UDQ::RequisiteEvaluationObjects& objects) const;

    /// Populate collection of requisite group level objects needed to
    /// evaluate this node.
    ///
    /// \param[in,out] objects Specific Schedule objects named in containing
    /// UDQ definition.  On exit also contains the specific group level
    /// objects needed by this node.  Includes group names or group name
    /// roots.
    void populateRequiredGroupObjects(UDQ::RequisiteEvaluationObjects& objects) const;

    /// Populate collection of requisite region level objects needed to
    /// evaluate this node.
    ///
    /// \param[in,out] objects Specific Schedule objects named in containing
    /// UDQ definition.  On exit also contains the specific region level
    /// objects needed by this node.  Includes region set names and,
    /// potentially, specific region numbers.
    void populateRequiredRegionObjects(UDQ::RequisiteEvaluationObjects& objects) const;

    /// Populate collection of requisite segment level objects needed to
    /// evaluate this node.
    ///
    /// \param[in,out] objects Specific Schedule objects named in containing
    /// UDQ definition.  On exit also contains the specific segment level
    /// objects needed by this node.  Includes well names and, potentially,
    /// specific segment numbers.
    void populateRequiredSegmentObjects(UDQ::RequisiteEvaluationObjects& objects) const;

    /// Populate collection of requisite well level objects needed to
    /// evaluate this node.
    ///
    /// \param[in,out] objects Specific Schedule objects named in containing
    /// UDQ definition.  On exit also contains the specific well level
    /// objects needed by this node.  Includes well names, well name
    /// templates, well lists, or well lists templates.
    void populateRequiredWellObjects(UDQ::RequisiteEvaluationObjects& objects) const;
};

UDQASTNode operator*(const UDQASTNode&lhs, double rhs);
UDQASTNode operator*(double lhs, const UDQASTNode& rhs);

} // namespace Opm

#endif // UDQASTNODE_HPP
