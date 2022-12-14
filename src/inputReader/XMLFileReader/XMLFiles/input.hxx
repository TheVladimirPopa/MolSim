// Copyright (c) 2005-2014 Code Synthesis Tools CC
//
// This program was generated by CodeSynthesis XSD, an XML Schema to
// C++ data binding compiler.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
// In addition, as a special exception, Code Synthesis Tools CC gives
// permission to link this program with the Xerces-C++ library (or with
// modified versions of Xerces-C++ that use the same license as Xerces-C++),
// and distribute linked combinations including the two. You must obey
// the GNU General Public License version 2 in all respects for all of
// the code used other than Xerces-C++. If you modify this copy of the
// program, you may extend this exception to your version of the program,
// but you are not obligated to do so. If you do not wish to do so, delete
// this exception statement from your version.
//
// Furthermore, Code Synthesis Tools CC makes a special exception for
// the Free/Libre and Open Source Software (FLOSS) which is described
// in the accompanying FLOSSE file.
//

#ifndef INPUT_HXX
#define INPUT_HXX

#ifndef XSD_CXX11
#define XSD_CXX11
#endif

#ifndef XSD_USE_CHAR
#define XSD_USE_CHAR
#endif

#ifndef XSD_CXX_TREE_USE_CHAR
#define XSD_CXX_TREE_USE_CHAR
#endif

// Begin prologue.
//
//
// End prologue.

#include <xsd/cxx/config.hxx>

#if (XSD_INT_VERSION != 4000000L)
#error XSD runtime version mismatch
#endif

#include <xsd/cxx/pre.hxx>

#include <xsd/cxx/xml/char-utf8.hxx>

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/types.hxx>

#include <xsd/cxx/xml/error-handler.hxx>

#include <xsd/cxx/xml/dom/auto-ptr.hxx>

#include <xsd/cxx/tree/parsing.hxx>
#include <xsd/cxx/tree/parsing/byte.hxx>
#include <xsd/cxx/tree/parsing/unsigned-byte.hxx>
#include <xsd/cxx/tree/parsing/short.hxx>
#include <xsd/cxx/tree/parsing/unsigned-short.hxx>
#include <xsd/cxx/tree/parsing/int.hxx>
#include <xsd/cxx/tree/parsing/unsigned-int.hxx>
#include <xsd/cxx/tree/parsing/long.hxx>
#include <xsd/cxx/tree/parsing/unsigned-long.hxx>
#include <xsd/cxx/tree/parsing/boolean.hxx>
#include <xsd/cxx/tree/parsing/float.hxx>
#include <xsd/cxx/tree/parsing/double.hxx>
#include <xsd/cxx/tree/parsing/decimal.hxx>

namespace xml_schema
{
  // anyType and anySimpleType.
  //
  typedef ::xsd::cxx::tree::type type;
  typedef ::xsd::cxx::tree::simple_type< char, type > simple_type;
  typedef ::xsd::cxx::tree::type container;

  // 8-bit
  //
  typedef signed char byte;
  typedef unsigned char unsigned_byte;

  // 16-bit
  //
  typedef short short_;
  typedef unsigned short unsigned_short;

  // 32-bit
  //
  typedef int int_;
  typedef unsigned int unsigned_int;

  // 64-bit
  //
  typedef long long long_;
  typedef unsigned long long unsigned_long;

  // Supposed to be arbitrary-length integral types.
  //
  typedef long long integer;
  typedef long long non_positive_integer;
  typedef unsigned long long non_negative_integer;
  typedef unsigned long long positive_integer;
  typedef long long negative_integer;

  // Boolean.
  //
  typedef bool boolean;

  // Floating-point types.
  //
  typedef float float_;
  typedef double double_;
  typedef double decimal;

  // String types.
  //
  typedef ::xsd::cxx::tree::string< char, simple_type > string;
  typedef ::xsd::cxx::tree::normalized_string< char, string > normalized_string;
  typedef ::xsd::cxx::tree::token< char, normalized_string > token;
  typedef ::xsd::cxx::tree::name< char, token > name;
  typedef ::xsd::cxx::tree::nmtoken< char, token > nmtoken;
  typedef ::xsd::cxx::tree::nmtokens< char, simple_type, nmtoken > nmtokens;
  typedef ::xsd::cxx::tree::ncname< char, name > ncname;
  typedef ::xsd::cxx::tree::language< char, token > language;

  // ID/IDREF.
  //
  typedef ::xsd::cxx::tree::id< char, ncname > id;
  typedef ::xsd::cxx::tree::idref< char, ncname, type > idref;
  typedef ::xsd::cxx::tree::idrefs< char, simple_type, idref > idrefs;

  // URI.
  //
  typedef ::xsd::cxx::tree::uri< char, simple_type > uri;

  // Qualified name.
  //
  typedef ::xsd::cxx::tree::qname< char, simple_type, uri, ncname > qname;

  // Binary.
  //
  typedef ::xsd::cxx::tree::buffer< char > buffer;
  typedef ::xsd::cxx::tree::base64_binary< char, simple_type > base64_binary;
  typedef ::xsd::cxx::tree::hex_binary< char, simple_type > hex_binary;

  // Date/time.
  //
  typedef ::xsd::cxx::tree::time_zone time_zone;
  typedef ::xsd::cxx::tree::date< char, simple_type > date;
  typedef ::xsd::cxx::tree::date_time< char, simple_type > date_time;
  typedef ::xsd::cxx::tree::duration< char, simple_type > duration;
  typedef ::xsd::cxx::tree::gday< char, simple_type > gday;
  typedef ::xsd::cxx::tree::gmonth< char, simple_type > gmonth;
  typedef ::xsd::cxx::tree::gmonth_day< char, simple_type > gmonth_day;
  typedef ::xsd::cxx::tree::gyear< char, simple_type > gyear;
  typedef ::xsd::cxx::tree::gyear_month< char, simple_type > gyear_month;
  typedef ::xsd::cxx::tree::time< char, simple_type > time;

  // Entity.
  //
  typedef ::xsd::cxx::tree::entity< char, ncname > entity;
  typedef ::xsd::cxx::tree::entities< char, simple_type, entity > entities;

  typedef ::xsd::cxx::tree::content_order content_order;
  // Flags and properties.
  //
  typedef ::xsd::cxx::tree::flags flags;
  typedef ::xsd::cxx::tree::properties< char > properties;

  // Parsing/serialization diagnostics.
  //
  typedef ::xsd::cxx::tree::severity severity;
  typedef ::xsd::cxx::tree::error< char > error;
  typedef ::xsd::cxx::tree::diagnostics< char > diagnostics;

  // Exceptions.
  //
  typedef ::xsd::cxx::tree::exception< char > exception;
  typedef ::xsd::cxx::tree::bounds< char > bounds;
  typedef ::xsd::cxx::tree::duplicate_id< char > duplicate_id;
  typedef ::xsd::cxx::tree::parsing< char > parsing;
  typedef ::xsd::cxx::tree::expected_element< char > expected_element;
  typedef ::xsd::cxx::tree::unexpected_element< char > unexpected_element;
  typedef ::xsd::cxx::tree::expected_attribute< char > expected_attribute;
  typedef ::xsd::cxx::tree::unexpected_enumerator< char > unexpected_enumerator;
  typedef ::xsd::cxx::tree::expected_text_content< char > expected_text_content;
  typedef ::xsd::cxx::tree::no_prefix_mapping< char > no_prefix_mapping;

  // Error handler callback interface.
  //
  typedef ::xsd::cxx::xml::error_handler< char > error_handler;

  // DOM interaction.
  //
  namespace dom
  {
    // Automatic pointer for DOMDocument.
    //
    using ::xsd::cxx::xml::dom::unique_ptr;

#ifndef XSD_CXX_TREE_TREE_NODE_KEY__XML_SCHEMA
#define XSD_CXX_TREE_TREE_NODE_KEY__XML_SCHEMA
    // DOM user data key for back pointers to tree nodes.
    //
    const XMLCh* const tree_node_key = ::xsd::cxx::tree::user_data_keys::node;
#endif
  }
}

// Forward declarations.
//
class simulation_t;
class array_i;
class array_d;
class cuboid_t;
class sphere_t;
class simTypes_t;
class input_t;
class containerTypes_t;
class linkedCellContainer_t;
class vectorContainer_t;
class boundary_t;

#include <memory>    // ::std::unique_ptr
#include <limits>    // std::numeric_limits
#include <algorithm> // std::binary_search
#include <utility>   // std::move

#include <xsd/cxx/xml/char-utf8.hxx>

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/containers.hxx>
#include <xsd/cxx/tree/list.hxx>

#include <xsd/cxx/xml/dom/parsing-header.hxx>

class simulation_t: public ::xml_schema::type
{
  public:
  // SimTypes
  //
  typedef ::simTypes_t SimTypes_type;
  typedef ::xsd::cxx::tree::sequence< SimTypes_type > SimTypes_sequence;
  typedef SimTypes_sequence::iterator SimTypes_iterator;
  typedef SimTypes_sequence::const_iterator SimTypes_const_iterator;
  typedef ::xsd::cxx::tree::traits< SimTypes_type, char > SimTypes_traits;

  const SimTypes_sequence&
  SimTypes () const;

  SimTypes_sequence&
  SimTypes ();

  void
  SimTypes (const SimTypes_sequence& s);

  // Container_T
  //
  typedef ::containerTypes_t Container_T_type;
  typedef ::xsd::cxx::tree::sequence< Container_T_type > Container_T_sequence;
  typedef Container_T_sequence::iterator Container_T_iterator;
  typedef Container_T_sequence::const_iterator Container_T_const_iterator;
  typedef ::xsd::cxx::tree::traits< Container_T_type, char > Container_T_traits;

  const Container_T_sequence&
  Container_T () const;

  Container_T_sequence&
  Container_T ();

  void
  Container_T (const Container_T_sequence& s);

  // InputFile
  //
  typedef ::input_t InputFile_type;
  typedef ::xsd::cxx::tree::sequence< InputFile_type > InputFile_sequence;
  typedef InputFile_sequence::iterator InputFile_iterator;
  typedef InputFile_sequence::const_iterator InputFile_const_iterator;
  typedef ::xsd::cxx::tree::traits< InputFile_type, char > InputFile_traits;

  const InputFile_sequence&
  InputFile () const;

  InputFile_sequence&
  InputFile ();

  void
  InputFile (const InputFile_sequence& s);

  // endTime
  //
  typedef ::xml_schema::double_ endTime_type;
  typedef ::xsd::cxx::tree::traits< endTime_type, char, ::xsd::cxx::tree::schema_type::double_ > endTime_traits;

  const endTime_type&
  endTime () const;

  endTime_type&
  endTime ();

  void
  endTime (const endTime_type& x);

  // deltaT
  //
  typedef ::xml_schema::double_ deltaT_type;
  typedef ::xsd::cxx::tree::traits< deltaT_type, char, ::xsd::cxx::tree::schema_type::double_ > deltaT_traits;

  const deltaT_type&
  deltaT () const;

  deltaT_type&
  deltaT ();

  void
  deltaT (const deltaT_type& x);

  // writeOutFrequency
  //
  typedef ::xml_schema::unsigned_int writeOutFrequency_type;
  typedef ::xsd::cxx::tree::traits< writeOutFrequency_type, char > writeOutFrequency_traits;

  const writeOutFrequency_type&
  writeOutFrequency () const;

  writeOutFrequency_type&
  writeOutFrequency ();

  void
  writeOutFrequency (const writeOutFrequency_type& x);

  // filename
  //
  typedef ::xml_schema::string filename_type;
  typedef ::xsd::cxx::tree::traits< filename_type, char > filename_traits;

  const filename_type&
  filename () const;

  filename_type&
  filename ();

  void
  filename (const filename_type& x);

  void
  filename (::std::unique_ptr< filename_type > p);

  // Constructors.
  //
  simulation_t (const endTime_type&,
                const deltaT_type&,
                const writeOutFrequency_type&,
                const filename_type&);

  simulation_t (const ::xercesc::DOMElement& e,
                ::xml_schema::flags f = 0,
                ::xml_schema::container* c = 0);

  simulation_t (const simulation_t& x,
                ::xml_schema::flags f = 0,
                ::xml_schema::container* c = 0);

  virtual simulation_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  simulation_t&
  operator= (const simulation_t& x);

  virtual 
  ~simulation_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  SimTypes_sequence SimTypes_;
  Container_T_sequence Container_T_;
  InputFile_sequence InputFile_;
  ::xsd::cxx::tree::one< endTime_type > endTime_;
  ::xsd::cxx::tree::one< deltaT_type > deltaT_;
  ::xsd::cxx::tree::one< writeOutFrequency_type > writeOutFrequency_;
  ::xsd::cxx::tree::one< filename_type > filename_;
};

class array_i: public ::xml_schema::type
{
  public:
  // x
  //
  typedef ::xml_schema::int_ x_type;
  typedef ::xsd::cxx::tree::traits< x_type, char > x_traits;

  const x_type&
  x () const;

  x_type&
  x ();

  void
  x (const x_type& x);

  // y
  //
  typedef ::xml_schema::int_ y_type;
  typedef ::xsd::cxx::tree::traits< y_type, char > y_traits;

  const y_type&
  y () const;

  y_type&
  y ();

  void
  y (const y_type& x);

  // z
  //
  typedef ::xml_schema::int_ z_type;
  typedef ::xsd::cxx::tree::traits< z_type, char > z_traits;

  const z_type&
  z () const;

  z_type&
  z ();

  void
  z (const z_type& x);

  // Constructors.
  //
  array_i (const x_type&,
           const y_type&,
           const z_type&);

  array_i (const ::xercesc::DOMElement& e,
           ::xml_schema::flags f = 0,
           ::xml_schema::container* c = 0);

  array_i (const array_i& x,
           ::xml_schema::flags f = 0,
           ::xml_schema::container* c = 0);

  virtual array_i*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  array_i&
  operator= (const array_i& x);

  virtual 
  ~array_i ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< x_type > x_;
  ::xsd::cxx::tree::one< y_type > y_;
  ::xsd::cxx::tree::one< z_type > z_;
};

class array_d: public ::xml_schema::type
{
  public:
  // x
  //
  typedef ::xml_schema::double_ x_type;
  typedef ::xsd::cxx::tree::traits< x_type, char, ::xsd::cxx::tree::schema_type::double_ > x_traits;

  const x_type&
  x () const;

  x_type&
  x ();

  void
  x (const x_type& x);

  // y
  //
  typedef ::xml_schema::double_ y_type;
  typedef ::xsd::cxx::tree::traits< y_type, char, ::xsd::cxx::tree::schema_type::double_ > y_traits;

  const y_type&
  y () const;

  y_type&
  y ();

  void
  y (const y_type& x);

  // z
  //
  typedef ::xml_schema::double_ z_type;
  typedef ::xsd::cxx::tree::traits< z_type, char, ::xsd::cxx::tree::schema_type::double_ > z_traits;

  const z_type&
  z () const;

  z_type&
  z ();

  void
  z (const z_type& x);

  // Constructors.
  //
  array_d (const x_type&,
           const y_type&,
           const z_type&);

  array_d (const ::xercesc::DOMElement& e,
           ::xml_schema::flags f = 0,
           ::xml_schema::container* c = 0);

  array_d (const array_d& x,
           ::xml_schema::flags f = 0,
           ::xml_schema::container* c = 0);

  virtual array_d*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  array_d&
  operator= (const array_d& x);

  virtual 
  ~array_d ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< x_type > x_;
  ::xsd::cxx::tree::one< y_type > y_;
  ::xsd::cxx::tree::one< z_type > z_;
};

class cuboid_t: public ::xml_schema::type
{
  public:
  // position
  //
  typedef ::array_d position_type;
  typedef ::xsd::cxx::tree::traits< position_type, char > position_traits;

  const position_type&
  position () const;

  position_type&
  position ();

  void
  position (const position_type& x);

  void
  position (::std::unique_ptr< position_type > p);

  // dimension
  //
  typedef ::array_i dimension_type;
  typedef ::xsd::cxx::tree::traits< dimension_type, char > dimension_traits;

  const dimension_type&
  dimension () const;

  dimension_type&
  dimension ();

  void
  dimension (const dimension_type& x);

  void
  dimension (::std::unique_ptr< dimension_type > p);

  // velocity
  //
  typedef ::array_d velocity_type;
  typedef ::xsd::cxx::tree::traits< velocity_type, char > velocity_traits;

  const velocity_type&
  velocity () const;

  velocity_type&
  velocity ();

  void
  velocity (const velocity_type& x);

  void
  velocity (::std::unique_ptr< velocity_type > p);

  // distance
  //
  typedef ::xml_schema::double_ distance_type;
  typedef ::xsd::cxx::tree::traits< distance_type, char, ::xsd::cxx::tree::schema_type::double_ > distance_traits;

  const distance_type&
  distance () const;

  distance_type&
  distance ();

  void
  distance (const distance_type& x);

  // mass
  //
  typedef ::xml_schema::double_ mass_type;
  typedef ::xsd::cxx::tree::traits< mass_type, char, ::xsd::cxx::tree::schema_type::double_ > mass_traits;

  const mass_type&
  mass () const;

  mass_type&
  mass ();

  void
  mass (const mass_type& x);

  // type
  //
  typedef ::xml_schema::int_ type_type;
  typedef ::xsd::cxx::tree::traits< type_type, char > type_traits;

  const type_type&
  type () const;

  type_type&
  type ();

  void
  type (const type_type& x);

  // Constructors.
  //
  cuboid_t (const position_type&,
            const dimension_type&,
            const velocity_type&,
            const distance_type&,
            const mass_type&,
            const type_type&);

  cuboid_t (::std::unique_ptr< position_type >,
            ::std::unique_ptr< dimension_type >,
            ::std::unique_ptr< velocity_type >,
            const distance_type&,
            const mass_type&,
            const type_type&);

  cuboid_t (const ::xercesc::DOMElement& e,
            ::xml_schema::flags f = 0,
            ::xml_schema::container* c = 0);

  cuboid_t (const cuboid_t& x,
            ::xml_schema::flags f = 0,
            ::xml_schema::container* c = 0);

  virtual cuboid_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  cuboid_t&
  operator= (const cuboid_t& x);

  virtual 
  ~cuboid_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< position_type > position_;
  ::xsd::cxx::tree::one< dimension_type > dimension_;
  ::xsd::cxx::tree::one< velocity_type > velocity_;
  ::xsd::cxx::tree::one< distance_type > distance_;
  ::xsd::cxx::tree::one< mass_type > mass_;
  ::xsd::cxx::tree::one< type_type > type_;
};

class sphere_t: public ::xml_schema::type
{
  public:
  // position
  //
  typedef ::array_d position_type;
  typedef ::xsd::cxx::tree::traits< position_type, char > position_traits;

  const position_type&
  position () const;

  position_type&
  position ();

  void
  position (const position_type& x);

  void
  position (::std::unique_ptr< position_type > p);

  // velocity
  //
  typedef ::array_d velocity_type;
  typedef ::xsd::cxx::tree::traits< velocity_type, char > velocity_traits;

  const velocity_type&
  velocity () const;

  velocity_type&
  velocity ();

  void
  velocity (const velocity_type& x);

  void
  velocity (::std::unique_ptr< velocity_type > p);

  // radius
  //
  typedef ::xml_schema::unsigned_int radius_type;
  typedef ::xsd::cxx::tree::traits< radius_type, char > radius_traits;

  const radius_type&
  radius () const;

  radius_type&
  radius ();

  void
  radius (const radius_type& x);

  // dimension
  //
  typedef ::xml_schema::unsigned_int dimension_type;
  typedef ::xsd::cxx::tree::traits< dimension_type, char > dimension_traits;

  const dimension_type&
  dimension () const;

  dimension_type&
  dimension ();

  void
  dimension (const dimension_type& x);

  // distance
  //
  typedef ::xml_schema::double_ distance_type;
  typedef ::xsd::cxx::tree::traits< distance_type, char, ::xsd::cxx::tree::schema_type::double_ > distance_traits;

  const distance_type&
  distance () const;

  distance_type&
  distance ();

  void
  distance (const distance_type& x);

  // mass
  //
  typedef ::xml_schema::double_ mass_type;
  typedef ::xsd::cxx::tree::traits< mass_type, char, ::xsd::cxx::tree::schema_type::double_ > mass_traits;

  const mass_type&
  mass () const;

  mass_type&
  mass ();

  void
  mass (const mass_type& x);

  // type
  //
  typedef ::xml_schema::int_ type_type;
  typedef ::xsd::cxx::tree::traits< type_type, char > type_traits;

  const type_type&
  type () const;

  type_type&
  type ();

  void
  type (const type_type& x);

  // Constructors.
  //
  sphere_t (const position_type&,
            const velocity_type&,
            const radius_type&,
            const dimension_type&,
            const distance_type&,
            const mass_type&,
            const type_type&);

  sphere_t (::std::unique_ptr< position_type >,
            ::std::unique_ptr< velocity_type >,
            const radius_type&,
            const dimension_type&,
            const distance_type&,
            const mass_type&,
            const type_type&);

  sphere_t (const ::xercesc::DOMElement& e,
            ::xml_schema::flags f = 0,
            ::xml_schema::container* c = 0);

  sphere_t (const sphere_t& x,
            ::xml_schema::flags f = 0,
            ::xml_schema::container* c = 0);

  virtual sphere_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  sphere_t&
  operator= (const sphere_t& x);

  virtual 
  ~sphere_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< position_type > position_;
  ::xsd::cxx::tree::one< velocity_type > velocity_;
  ::xsd::cxx::tree::one< radius_type > radius_;
  ::xsd::cxx::tree::one< dimension_type > dimension_;
  ::xsd::cxx::tree::one< distance_type > distance_;
  ::xsd::cxx::tree::one< mass_type > mass_;
  ::xsd::cxx::tree::one< type_type > type_;
};

class simTypes_t: public ::xml_schema::type
{
  public:
  // Cuboid
  //
  typedef ::cuboid_t Cuboid_type;
  typedef ::xsd::cxx::tree::sequence< Cuboid_type > Cuboid_sequence;
  typedef Cuboid_sequence::iterator Cuboid_iterator;
  typedef Cuboid_sequence::const_iterator Cuboid_const_iterator;
  typedef ::xsd::cxx::tree::traits< Cuboid_type, char > Cuboid_traits;

  const Cuboid_sequence&
  Cuboid () const;

  Cuboid_sequence&
  Cuboid ();

  void
  Cuboid (const Cuboid_sequence& s);

  // Sphere
  //
  typedef ::sphere_t Sphere_type;
  typedef ::xsd::cxx::tree::sequence< Sphere_type > Sphere_sequence;
  typedef Sphere_sequence::iterator Sphere_iterator;
  typedef Sphere_sequence::const_iterator Sphere_const_iterator;
  typedef ::xsd::cxx::tree::traits< Sphere_type, char > Sphere_traits;

  const Sphere_sequence&
  Sphere () const;

  Sphere_sequence&
  Sphere ();

  void
  Sphere (const Sphere_sequence& s);

  // Constructors.
  //
  simTypes_t ();

  simTypes_t (const ::xercesc::DOMElement& e,
              ::xml_schema::flags f = 0,
              ::xml_schema::container* c = 0);

  simTypes_t (const simTypes_t& x,
              ::xml_schema::flags f = 0,
              ::xml_schema::container* c = 0);

  virtual simTypes_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  simTypes_t&
  operator= (const simTypes_t& x);

  virtual 
  ~simTypes_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  Cuboid_sequence Cuboid_;
  Sphere_sequence Sphere_;
};

class input_t: public ::xml_schema::type
{
  public:
  // path
  //
  typedef ::xml_schema::string path_type;
  typedef ::xsd::cxx::tree::traits< path_type, char > path_traits;

  const path_type&
  path () const;

  path_type&
  path ();

  void
  path (const path_type& x);

  void
  path (::std::unique_ptr< path_type > p);

  // Constructors.
  //
  input_t (const path_type&);

  input_t (const ::xercesc::DOMElement& e,
           ::xml_schema::flags f = 0,
           ::xml_schema::container* c = 0);

  input_t (const input_t& x,
           ::xml_schema::flags f = 0,
           ::xml_schema::container* c = 0);

  virtual input_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  input_t&
  operator= (const input_t& x);

  virtual 
  ~input_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< path_type > path_;
};

class containerTypes_t: public ::xml_schema::type
{
  public:
  // LinkedCell
  //
  typedef ::linkedCellContainer_t LinkedCell_type;
  typedef ::xsd::cxx::tree::sequence< LinkedCell_type > LinkedCell_sequence;
  typedef LinkedCell_sequence::iterator LinkedCell_iterator;
  typedef LinkedCell_sequence::const_iterator LinkedCell_const_iterator;
  typedef ::xsd::cxx::tree::traits< LinkedCell_type, char > LinkedCell_traits;

  const LinkedCell_sequence&
  LinkedCell () const;

  LinkedCell_sequence&
  LinkedCell ();

  void
  LinkedCell (const LinkedCell_sequence& s);

  // VectorCont
  //
  typedef ::vectorContainer_t VectorCont_type;
  typedef ::xsd::cxx::tree::sequence< VectorCont_type > VectorCont_sequence;
  typedef VectorCont_sequence::iterator VectorCont_iterator;
  typedef VectorCont_sequence::const_iterator VectorCont_const_iterator;
  typedef ::xsd::cxx::tree::traits< VectorCont_type, char > VectorCont_traits;

  const VectorCont_sequence&
  VectorCont () const;

  VectorCont_sequence&
  VectorCont ();

  void
  VectorCont (const VectorCont_sequence& s);

  // Constructors.
  //
  containerTypes_t ();

  containerTypes_t (const ::xercesc::DOMElement& e,
                    ::xml_schema::flags f = 0,
                    ::xml_schema::container* c = 0);

  containerTypes_t (const containerTypes_t& x,
                    ::xml_schema::flags f = 0,
                    ::xml_schema::container* c = 0);

  virtual containerTypes_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  containerTypes_t&
  operator= (const containerTypes_t& x);

  virtual 
  ~containerTypes_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  LinkedCell_sequence LinkedCell_;
  VectorCont_sequence VectorCont_;
};

class linkedCellContainer_t: public ::xml_schema::type
{
  public:
  // leftLowerBound
  //
  typedef ::array_d leftLowerBound_type;
  typedef ::xsd::cxx::tree::traits< leftLowerBound_type, char > leftLowerBound_traits;

  const leftLowerBound_type&
  leftLowerBound () const;

  leftLowerBound_type&
  leftLowerBound ();

  void
  leftLowerBound (const leftLowerBound_type& x);

  void
  leftLowerBound (::std::unique_ptr< leftLowerBound_type > p);

  // rightUpperBound
  //
  typedef ::array_d rightUpperBound_type;
  typedef ::xsd::cxx::tree::traits< rightUpperBound_type, char > rightUpperBound_traits;

  const rightUpperBound_type&
  rightUpperBound () const;

  rightUpperBound_type&
  rightUpperBound ();

  void
  rightUpperBound (const rightUpperBound_type& x);

  void
  rightUpperBound (::std::unique_ptr< rightUpperBound_type > p);

  // left
  //
  typedef ::boundary_t left_type;
  typedef ::xsd::cxx::tree::traits< left_type, char > left_traits;

  const left_type&
  left () const;

  left_type&
  left ();

  void
  left (const left_type& x);

  void
  left (::std::unique_ptr< left_type > p);

  // right
  //
  typedef ::boundary_t right_type;
  typedef ::xsd::cxx::tree::traits< right_type, char > right_traits;

  const right_type&
  right () const;

  right_type&
  right ();

  void
  right (const right_type& x);

  void
  right (::std::unique_ptr< right_type > p);

  // top
  //
  typedef ::boundary_t top_type;
  typedef ::xsd::cxx::tree::traits< top_type, char > top_traits;

  const top_type&
  top () const;

  top_type&
  top ();

  void
  top (const top_type& x);

  void
  top (::std::unique_ptr< top_type > p);

  // bottom
  //
  typedef ::boundary_t bottom_type;
  typedef ::xsd::cxx::tree::traits< bottom_type, char > bottom_traits;

  const bottom_type&
  bottom () const;

  bottom_type&
  bottom ();

  void
  bottom (const bottom_type& x);

  void
  bottom (::std::unique_ptr< bottom_type > p);

  // front
  //
  typedef ::boundary_t front_type;
  typedef ::xsd::cxx::tree::traits< front_type, char > front_traits;

  const front_type&
  front () const;

  front_type&
  front ();

  void
  front (const front_type& x);

  void
  front (::std::unique_ptr< front_type > p);

  // back
  //
  typedef ::boundary_t back_type;
  typedef ::xsd::cxx::tree::traits< back_type, char > back_traits;

  const back_type&
  back () const;

  back_type&
  back ();

  void
  back (const back_type& x);

  void
  back (::std::unique_ptr< back_type > p);

  // cellSize
  //
  typedef ::xml_schema::double_ cellSize_type;
  typedef ::xsd::cxx::tree::traits< cellSize_type, char, ::xsd::cxx::tree::schema_type::double_ > cellSize_traits;

  const cellSize_type&
  cellSize () const;

  cellSize_type&
  cellSize ();

  void
  cellSize (const cellSize_type& x);

  // cutOffRadius
  //
  typedef ::xml_schema::double_ cutOffRadius_type;
  typedef ::xsd::cxx::tree::traits< cutOffRadius_type, char, ::xsd::cxx::tree::schema_type::double_ > cutOffRadius_traits;

  const cutOffRadius_type&
  cutOffRadius () const;

  cutOffRadius_type&
  cutOffRadius ();

  void
  cutOffRadius (const cutOffRadius_type& x);

  // Constructors.
  //
  linkedCellContainer_t (const leftLowerBound_type&,
                         const rightUpperBound_type&,
                         const left_type&,
                         const right_type&,
                         const top_type&,
                         const bottom_type&,
                         const front_type&,
                         const back_type&,
                         const cellSize_type&,
                         const cutOffRadius_type&);

  linkedCellContainer_t (::std::unique_ptr< leftLowerBound_type >,
                         ::std::unique_ptr< rightUpperBound_type >,
                         const left_type&,
                         const right_type&,
                         const top_type&,
                         const bottom_type&,
                         const front_type&,
                         const back_type&,
                         const cellSize_type&,
                         const cutOffRadius_type&);

  linkedCellContainer_t (const ::xercesc::DOMElement& e,
                         ::xml_schema::flags f = 0,
                         ::xml_schema::container* c = 0);

  linkedCellContainer_t (const linkedCellContainer_t& x,
                         ::xml_schema::flags f = 0,
                         ::xml_schema::container* c = 0);

  virtual linkedCellContainer_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  linkedCellContainer_t&
  operator= (const linkedCellContainer_t& x);

  virtual 
  ~linkedCellContainer_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< leftLowerBound_type > leftLowerBound_;
  ::xsd::cxx::tree::one< rightUpperBound_type > rightUpperBound_;
  ::xsd::cxx::tree::one< left_type > left_;
  ::xsd::cxx::tree::one< right_type > right_;
  ::xsd::cxx::tree::one< top_type > top_;
  ::xsd::cxx::tree::one< bottom_type > bottom_;
  ::xsd::cxx::tree::one< front_type > front_;
  ::xsd::cxx::tree::one< back_type > back_;
  ::xsd::cxx::tree::one< cellSize_type > cellSize_;
  ::xsd::cxx::tree::one< cutOffRadius_type > cutOffRadius_;
};

class vectorContainer_t: public ::xml_schema::type
{
  public:
  // vector
  //
  typedef ::array_d vector_type;
  typedef ::xsd::cxx::tree::traits< vector_type, char > vector_traits;

  const vector_type&
  vector () const;

  vector_type&
  vector ();

  void
  vector (const vector_type& x);

  void
  vector (::std::unique_ptr< vector_type > p);

  // Constructors.
  //
  vectorContainer_t (const vector_type&);

  vectorContainer_t (::std::unique_ptr< vector_type >);

  vectorContainer_t (const ::xercesc::DOMElement& e,
                     ::xml_schema::flags f = 0,
                     ::xml_schema::container* c = 0);

  vectorContainer_t (const vectorContainer_t& x,
                     ::xml_schema::flags f = 0,
                     ::xml_schema::container* c = 0);

  virtual vectorContainer_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  vectorContainer_t&
  operator= (const vectorContainer_t& x);

  virtual 
  ~vectorContainer_t ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< vector_type > vector_;
};

class boundary_t: public ::xml_schema::string
{
  public:
  enum value
  {
    OUTFLOW,
    REFLECT,
    PERIODIC
  };

  boundary_t (value v);

  boundary_t (const char* v);

  boundary_t (const ::std::string& v);

  boundary_t (const ::xml_schema::string& v);

  boundary_t (const ::xercesc::DOMElement& e,
              ::xml_schema::flags f = 0,
              ::xml_schema::container* c = 0);

  boundary_t (const ::xercesc::DOMAttr& a,
              ::xml_schema::flags f = 0,
              ::xml_schema::container* c = 0);

  boundary_t (const ::std::string& s,
              const ::xercesc::DOMElement* e,
              ::xml_schema::flags f = 0,
              ::xml_schema::container* c = 0);

  boundary_t (const boundary_t& x,
              ::xml_schema::flags f = 0,
              ::xml_schema::container* c = 0);

  virtual boundary_t*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  boundary_t&
  operator= (value v);

  virtual
  operator value () const
  {
    return _xsd_boundary_t_convert ();
  }

  protected:
  value
  _xsd_boundary_t_convert () const;

  public:
  static const char* const _xsd_boundary_t_literals_[3];
  static const value _xsd_boundary_t_indexes_[3];
};

#include <iosfwd>

#include <xercesc/sax/InputSource.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

// Parse a URI or a local file.
//

::std::unique_ptr< ::simulation_t >
Simulation_XML (const ::std::string& uri,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::simulation_t >
Simulation_XML (const ::std::string& uri,
                ::xml_schema::error_handler& eh,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::simulation_t >
Simulation_XML (const ::std::string& uri,
                ::xercesc::DOMErrorHandler& eh,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

// Parse std::istream.
//

::std::unique_ptr< ::simulation_t >
Simulation_XML (::std::istream& is,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::simulation_t >
Simulation_XML (::std::istream& is,
                ::xml_schema::error_handler& eh,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::simulation_t >
Simulation_XML (::std::istream& is,
                ::xercesc::DOMErrorHandler& eh,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::simulation_t >
Simulation_XML (::std::istream& is,
                const ::std::string& id,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::simulation_t >
Simulation_XML (::std::istream& is,
                const ::std::string& id,
                ::xml_schema::error_handler& eh,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::simulation_t >
Simulation_XML (::std::istream& is,
                const ::std::string& id,
                ::xercesc::DOMErrorHandler& eh,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

// Parse xercesc::InputSource.
//

::std::unique_ptr< ::simulation_t >
Simulation_XML (::xercesc::InputSource& is,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::simulation_t >
Simulation_XML (::xercesc::InputSource& is,
                ::xml_schema::error_handler& eh,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::simulation_t >
Simulation_XML (::xercesc::InputSource& is,
                ::xercesc::DOMErrorHandler& eh,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

// Parse xercesc::DOMDocument.
//

::std::unique_ptr< ::simulation_t >
Simulation_XML (const ::xercesc::DOMDocument& d,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::simulation_t >
Simulation_XML (::xml_schema::dom::unique_ptr< ::xercesc::DOMDocument > d,
                ::xml_schema::flags f = 0,
                const ::xml_schema::properties& p = ::xml_schema::properties ());

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
//
// End epilogue.

#endif // INPUT_HXX
