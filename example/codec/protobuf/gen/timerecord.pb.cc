// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: timerecord.proto

#include "timerecord.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)
namespace gen {
namespace proto {
class TimeRecordDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<TimeRecord>
      _instance;
} _TimeRecord_default_instance_;
}  // namespace proto
}  // namespace gen
namespace protobuf_timerecord_2eproto {
void InitDefaultsTimeRecordImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::gen::proto::_TimeRecord_default_instance_;
    new (ptr) ::gen::proto::TimeRecord();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::gen::proto::TimeRecord::InitAsDefaultInstance();
}

void InitDefaultsTimeRecord() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultsTimeRecordImpl);
}

::google::protobuf::Metadata file_level_metadata[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::gen::proto::TimeRecord, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::gen::proto::TimeRecord, time_records_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::gen::proto::TimeRecord)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::gen::proto::_TimeRecord_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "timerecord.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\020timerecord.proto\022\tgen.proto\"\"\n\nTimeRec"
      "ord\022\024\n\014time_records\030\001 \003(\003b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 73);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "timerecord.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_timerecord_2eproto
namespace gen {
namespace proto {

// ===================================================================

void TimeRecord::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int TimeRecord::kTimeRecordsFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

TimeRecord::TimeRecord()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_timerecord_2eproto::InitDefaultsTimeRecord();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:gen.proto.TimeRecord)
}
TimeRecord::TimeRecord(const TimeRecord& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      time_records_(from.time_records_),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:gen.proto.TimeRecord)
}

void TimeRecord::SharedCtor() {
  _cached_size_ = 0;
}

TimeRecord::~TimeRecord() {
  // @@protoc_insertion_point(destructor:gen.proto.TimeRecord)
  SharedDtor();
}

void TimeRecord::SharedDtor() {
}

void TimeRecord::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* TimeRecord::descriptor() {
  ::protobuf_timerecord_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_timerecord_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const TimeRecord& TimeRecord::default_instance() {
  ::protobuf_timerecord_2eproto::InitDefaultsTimeRecord();
  return *internal_default_instance();
}

TimeRecord* TimeRecord::New(::google::protobuf::Arena* arena) const {
  TimeRecord* n = new TimeRecord;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void TimeRecord::Clear() {
// @@protoc_insertion_point(message_clear_start:gen.proto.TimeRecord)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  time_records_.Clear();
  _internal_metadata_.Clear();
}

bool TimeRecord::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:gen.proto.TimeRecord)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated int64 time_records = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitive<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 input, this->mutable_time_records())));
        } else if (
            static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(8u /* 8 & 0xFF */)) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitiveNoInline<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 1, 10u, input, this->mutable_time_records())));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:gen.proto.TimeRecord)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:gen.proto.TimeRecord)
  return false;
#undef DO_
}

void TimeRecord::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:gen.proto.TimeRecord)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated int64 time_records = 1;
  if (this->time_records_size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteTag(1, ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
    output->WriteVarint32(static_cast< ::google::protobuf::uint32>(
        _time_records_cached_byte_size_));
  }
  for (int i = 0, n = this->time_records_size(); i < n; i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt64NoTag(
      this->time_records(i), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:gen.proto.TimeRecord)
}

::google::protobuf::uint8* TimeRecord::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:gen.proto.TimeRecord)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated int64 time_records = 1;
  if (this->time_records_size() > 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteTagToArray(
      1,
      ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
      target);
    target = ::google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(
        static_cast< ::google::protobuf::int32>(
            _time_records_cached_byte_size_), target);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt64NoTagToArray(this->time_records_, target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:gen.proto.TimeRecord)
  return target;
}

size_t TimeRecord::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:gen.proto.TimeRecord)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // repeated int64 time_records = 1;
  {
    size_t data_size = ::google::protobuf::internal::WireFormatLite::
      Int64Size(this->time_records_);
    if (data_size > 0) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
            static_cast< ::google::protobuf::int32>(data_size));
    }
    int cached_size = ::google::protobuf::internal::ToCachedSize(data_size);
    GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
    _time_records_cached_byte_size_ = cached_size;
    GOOGLE_SAFE_CONCURRENT_WRITES_END();
    total_size += data_size;
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void TimeRecord::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:gen.proto.TimeRecord)
  GOOGLE_DCHECK_NE(&from, this);
  const TimeRecord* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const TimeRecord>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:gen.proto.TimeRecord)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:gen.proto.TimeRecord)
    MergeFrom(*source);
  }
}

void TimeRecord::MergeFrom(const TimeRecord& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:gen.proto.TimeRecord)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  time_records_.MergeFrom(from.time_records_);
}

void TimeRecord::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:gen.proto.TimeRecord)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void TimeRecord::CopyFrom(const TimeRecord& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:gen.proto.TimeRecord)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool TimeRecord::IsInitialized() const {
  return true;
}

void TimeRecord::Swap(TimeRecord* other) {
  if (other == this) return;
  InternalSwap(other);
}
void TimeRecord::InternalSwap(TimeRecord* other) {
  using std::swap;
  time_records_.InternalSwap(&other->time_records_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata TimeRecord::GetMetadata() const {
  protobuf_timerecord_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_timerecord_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace proto
}  // namespace gen

// @@protoc_insertion_point(global_scope)
