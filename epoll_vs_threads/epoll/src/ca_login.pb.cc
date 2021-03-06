// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "ca_login.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace CA {

namespace {

const ::google::protobuf::Descriptor* pb_MSG_SYS_CA_USER_LOGIN_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  pb_MSG_SYS_CA_USER_LOGIN_reflection_ = NULL;
const ::google::protobuf::Descriptor* pb_MSG_SYS_CA_USER_LOGIN_ACK_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  pb_MSG_SYS_CA_USER_LOGIN_ACK_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_ca_5flogin_2eproto() {
  protobuf_AddDesc_ca_5flogin_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "ca_login.proto");
  GOOGLE_CHECK(file != NULL);
  pb_MSG_SYS_CA_USER_LOGIN_descriptor_ = file->message_type(0);
  static const int pb_MSG_SYS_CA_USER_LOGIN_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(pb_MSG_SYS_CA_USER_LOGIN, name_),
  };
  pb_MSG_SYS_CA_USER_LOGIN_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      pb_MSG_SYS_CA_USER_LOGIN_descriptor_,
      pb_MSG_SYS_CA_USER_LOGIN::default_instance_,
      pb_MSG_SYS_CA_USER_LOGIN_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(pb_MSG_SYS_CA_USER_LOGIN, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(pb_MSG_SYS_CA_USER_LOGIN, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(pb_MSG_SYS_CA_USER_LOGIN));
  pb_MSG_SYS_CA_USER_LOGIN_ACK_descriptor_ = file->message_type(1);
  static const int pb_MSG_SYS_CA_USER_LOGIN_ACK_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(pb_MSG_SYS_CA_USER_LOGIN_ACK, token_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(pb_MSG_SYS_CA_USER_LOGIN_ACK, password_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(pb_MSG_SYS_CA_USER_LOGIN_ACK, user_id_),
  };
  pb_MSG_SYS_CA_USER_LOGIN_ACK_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      pb_MSG_SYS_CA_USER_LOGIN_ACK_descriptor_,
      pb_MSG_SYS_CA_USER_LOGIN_ACK::default_instance_,
      pb_MSG_SYS_CA_USER_LOGIN_ACK_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(pb_MSG_SYS_CA_USER_LOGIN_ACK, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(pb_MSG_SYS_CA_USER_LOGIN_ACK, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(pb_MSG_SYS_CA_USER_LOGIN_ACK));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_ca_5flogin_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    pb_MSG_SYS_CA_USER_LOGIN_descriptor_, &pb_MSG_SYS_CA_USER_LOGIN::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    pb_MSG_SYS_CA_USER_LOGIN_ACK_descriptor_, &pb_MSG_SYS_CA_USER_LOGIN_ACK::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_ca_5flogin_2eproto() {
  delete pb_MSG_SYS_CA_USER_LOGIN::default_instance_;
  delete pb_MSG_SYS_CA_USER_LOGIN_reflection_;
  delete pb_MSG_SYS_CA_USER_LOGIN_ACK::default_instance_;
  delete pb_MSG_SYS_CA_USER_LOGIN_ACK_reflection_;
}

void protobuf_AddDesc_ca_5flogin_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\016ca_login.proto\022\002CA\"(\n\030pb_MSG_SYS_CA_US"
    "ER_LOGIN\022\014\n\004name\030\001 \002(\t\"P\n\034pb_MSG_SYS_CA_"
    "USER_LOGIN_ACK\022\r\n\005token\030\001 \002(\t\022\020\n\010passwor"
    "d\030\002 \002(\t\022\017\n\007user_id\030\003 \002(\004", 144);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "ca_login.proto", &protobuf_RegisterTypes);
  pb_MSG_SYS_CA_USER_LOGIN::default_instance_ = new pb_MSG_SYS_CA_USER_LOGIN();
  pb_MSG_SYS_CA_USER_LOGIN_ACK::default_instance_ = new pb_MSG_SYS_CA_USER_LOGIN_ACK();
  pb_MSG_SYS_CA_USER_LOGIN::default_instance_->InitAsDefaultInstance();
  pb_MSG_SYS_CA_USER_LOGIN_ACK::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_ca_5flogin_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_ca_5flogin_2eproto {
  StaticDescriptorInitializer_ca_5flogin_2eproto() {
    protobuf_AddDesc_ca_5flogin_2eproto();
  }
} static_descriptor_initializer_ca_5flogin_2eproto_;


// ===================================================================

#ifndef _MSC_VER
const int pb_MSG_SYS_CA_USER_LOGIN::kNameFieldNumber;
#endif  // !_MSC_VER

pb_MSG_SYS_CA_USER_LOGIN::pb_MSG_SYS_CA_USER_LOGIN()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void pb_MSG_SYS_CA_USER_LOGIN::InitAsDefaultInstance() {
}

pb_MSG_SYS_CA_USER_LOGIN::pb_MSG_SYS_CA_USER_LOGIN(const pb_MSG_SYS_CA_USER_LOGIN& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void pb_MSG_SYS_CA_USER_LOGIN::SharedCtor() {
  _cached_size_ = 0;
  name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

pb_MSG_SYS_CA_USER_LOGIN::~pb_MSG_SYS_CA_USER_LOGIN() {
  SharedDtor();
}

void pb_MSG_SYS_CA_USER_LOGIN::SharedDtor() {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    delete name_;
  }
  if (this != default_instance_) {
  }
}

void pb_MSG_SYS_CA_USER_LOGIN::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* pb_MSG_SYS_CA_USER_LOGIN::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return pb_MSG_SYS_CA_USER_LOGIN_descriptor_;
}

const pb_MSG_SYS_CA_USER_LOGIN& pb_MSG_SYS_CA_USER_LOGIN::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_ca_5flogin_2eproto();  return *default_instance_;
}

pb_MSG_SYS_CA_USER_LOGIN* pb_MSG_SYS_CA_USER_LOGIN::default_instance_ = NULL;

pb_MSG_SYS_CA_USER_LOGIN* pb_MSG_SYS_CA_USER_LOGIN::New() const {
  return new pb_MSG_SYS_CA_USER_LOGIN;
}

void pb_MSG_SYS_CA_USER_LOGIN::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_name()) {
      if (name_ != &::google::protobuf::internal::kEmptyString) {
        name_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool pb_MSG_SYS_CA_USER_LOGIN::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string name = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->name().data(), this->name().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void pb_MSG_SYS_CA_USER_LOGIN::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string name = 1;
  if (has_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->name().data(), this->name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->name(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* pb_MSG_SYS_CA_USER_LOGIN::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string name = 1;
  if (has_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->name().data(), this->name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->name(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int pb_MSG_SYS_CA_USER_LOGIN::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string name = 1;
    if (has_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->name());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void pb_MSG_SYS_CA_USER_LOGIN::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const pb_MSG_SYS_CA_USER_LOGIN* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const pb_MSG_SYS_CA_USER_LOGIN*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void pb_MSG_SYS_CA_USER_LOGIN::MergeFrom(const pb_MSG_SYS_CA_USER_LOGIN& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_name()) {
      set_name(from.name());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void pb_MSG_SYS_CA_USER_LOGIN::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void pb_MSG_SYS_CA_USER_LOGIN::CopyFrom(const pb_MSG_SYS_CA_USER_LOGIN& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool pb_MSG_SYS_CA_USER_LOGIN::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void pb_MSG_SYS_CA_USER_LOGIN::Swap(pb_MSG_SYS_CA_USER_LOGIN* other) {
  if (other != this) {
    std::swap(name_, other->name_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata pb_MSG_SYS_CA_USER_LOGIN::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = pb_MSG_SYS_CA_USER_LOGIN_descriptor_;
  metadata.reflection = pb_MSG_SYS_CA_USER_LOGIN_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int pb_MSG_SYS_CA_USER_LOGIN_ACK::kTokenFieldNumber;
const int pb_MSG_SYS_CA_USER_LOGIN_ACK::kPasswordFieldNumber;
const int pb_MSG_SYS_CA_USER_LOGIN_ACK::kUserIdFieldNumber;
#endif  // !_MSC_VER

pb_MSG_SYS_CA_USER_LOGIN_ACK::pb_MSG_SYS_CA_USER_LOGIN_ACK()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void pb_MSG_SYS_CA_USER_LOGIN_ACK::InitAsDefaultInstance() {
}

pb_MSG_SYS_CA_USER_LOGIN_ACK::pb_MSG_SYS_CA_USER_LOGIN_ACK(const pb_MSG_SYS_CA_USER_LOGIN_ACK& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void pb_MSG_SYS_CA_USER_LOGIN_ACK::SharedCtor() {
  _cached_size_ = 0;
  token_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  password_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  user_id_ = GOOGLE_ULONGLONG(0);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

pb_MSG_SYS_CA_USER_LOGIN_ACK::~pb_MSG_SYS_CA_USER_LOGIN_ACK() {
  SharedDtor();
}

void pb_MSG_SYS_CA_USER_LOGIN_ACK::SharedDtor() {
  if (token_ != &::google::protobuf::internal::kEmptyString) {
    delete token_;
  }
  if (password_ != &::google::protobuf::internal::kEmptyString) {
    delete password_;
  }
  if (this != default_instance_) {
  }
}

void pb_MSG_SYS_CA_USER_LOGIN_ACK::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* pb_MSG_SYS_CA_USER_LOGIN_ACK::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return pb_MSG_SYS_CA_USER_LOGIN_ACK_descriptor_;
}

const pb_MSG_SYS_CA_USER_LOGIN_ACK& pb_MSG_SYS_CA_USER_LOGIN_ACK::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_ca_5flogin_2eproto();  return *default_instance_;
}

pb_MSG_SYS_CA_USER_LOGIN_ACK* pb_MSG_SYS_CA_USER_LOGIN_ACK::default_instance_ = NULL;

pb_MSG_SYS_CA_USER_LOGIN_ACK* pb_MSG_SYS_CA_USER_LOGIN_ACK::New() const {
  return new pb_MSG_SYS_CA_USER_LOGIN_ACK;
}

void pb_MSG_SYS_CA_USER_LOGIN_ACK::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_token()) {
      if (token_ != &::google::protobuf::internal::kEmptyString) {
        token_->clear();
      }
    }
    if (has_password()) {
      if (password_ != &::google::protobuf::internal::kEmptyString) {
        password_->clear();
      }
    }
    user_id_ = GOOGLE_ULONGLONG(0);
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool pb_MSG_SYS_CA_USER_LOGIN_ACK::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string token = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_token()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->token().data(), this->token().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_password;
        break;
      }
      
      // required string password = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_password:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_password()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->password().data(), this->password().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_user_id;
        break;
      }
      
      // required uint64 user_id = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_user_id:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &user_id_)));
          set_has_user_id();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void pb_MSG_SYS_CA_USER_LOGIN_ACK::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string token = 1;
  if (has_token()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->token().data(), this->token().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->token(), output);
  }
  
  // required string password = 2;
  if (has_password()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->password().data(), this->password().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->password(), output);
  }
  
  // required uint64 user_id = 3;
  if (has_user_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(3, this->user_id(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* pb_MSG_SYS_CA_USER_LOGIN_ACK::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string token = 1;
  if (has_token()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->token().data(), this->token().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->token(), target);
  }
  
  // required string password = 2;
  if (has_password()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->password().data(), this->password().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->password(), target);
  }
  
  // required uint64 user_id = 3;
  if (has_user_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(3, this->user_id(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int pb_MSG_SYS_CA_USER_LOGIN_ACK::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string token = 1;
    if (has_token()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->token());
    }
    
    // required string password = 2;
    if (has_password()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->password());
    }
    
    // required uint64 user_id = 3;
    if (has_user_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->user_id());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void pb_MSG_SYS_CA_USER_LOGIN_ACK::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const pb_MSG_SYS_CA_USER_LOGIN_ACK* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const pb_MSG_SYS_CA_USER_LOGIN_ACK*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void pb_MSG_SYS_CA_USER_LOGIN_ACK::MergeFrom(const pb_MSG_SYS_CA_USER_LOGIN_ACK& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_token()) {
      set_token(from.token());
    }
    if (from.has_password()) {
      set_password(from.password());
    }
    if (from.has_user_id()) {
      set_user_id(from.user_id());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void pb_MSG_SYS_CA_USER_LOGIN_ACK::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void pb_MSG_SYS_CA_USER_LOGIN_ACK::CopyFrom(const pb_MSG_SYS_CA_USER_LOGIN_ACK& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool pb_MSG_SYS_CA_USER_LOGIN_ACK::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000007) != 0x00000007) return false;
  
  return true;
}

void pb_MSG_SYS_CA_USER_LOGIN_ACK::Swap(pb_MSG_SYS_CA_USER_LOGIN_ACK* other) {
  if (other != this) {
    std::swap(token_, other->token_);
    std::swap(password_, other->password_);
    std::swap(user_id_, other->user_id_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata pb_MSG_SYS_CA_USER_LOGIN_ACK::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = pb_MSG_SYS_CA_USER_LOGIN_ACK_descriptor_;
  metadata.reflection = pb_MSG_SYS_CA_USER_LOGIN_ACK_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace CA

// @@protoc_insertion_point(global_scope)
