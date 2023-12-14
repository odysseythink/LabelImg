// Code generated by protoc-gen-go. DO NOT EDIT.
// source: api.proto

package pb

import (
	fmt "fmt"
	proto "github.com/golang/protobuf/proto"
	math "math"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion3 // please upgrade the proto package

type PK_PASCAL_VOC_ANNOTATION_OBJ_CMD_PASCAL_VOC_ANNOTATION_OBJ int32

const (
	PK_PASCAL_VOC_ANNOTATION_OBJ_UNKNOWN PK_PASCAL_VOC_ANNOTATION_OBJ_CMD_PASCAL_VOC_ANNOTATION_OBJ = 0
	PK_PASCAL_VOC_ANNOTATION_OBJ_CMD     PK_PASCAL_VOC_ANNOTATION_OBJ_CMD_PASCAL_VOC_ANNOTATION_OBJ = 168493057
)

var PK_PASCAL_VOC_ANNOTATION_OBJ_CMD_PASCAL_VOC_ANNOTATION_OBJ_name = map[int32]string{
	0:         "UNKNOWN",
	168493057: "CMD",
}

var PK_PASCAL_VOC_ANNOTATION_OBJ_CMD_PASCAL_VOC_ANNOTATION_OBJ_value = map[string]int32{
	"UNKNOWN": 0,
	"CMD":     168493057,
}

func (x PK_PASCAL_VOC_ANNOTATION_OBJ_CMD_PASCAL_VOC_ANNOTATION_OBJ) String() string {
	return proto.EnumName(PK_PASCAL_VOC_ANNOTATION_OBJ_CMD_PASCAL_VOC_ANNOTATION_OBJ_name, int32(x))
}

func (PK_PASCAL_VOC_ANNOTATION_OBJ_CMD_PASCAL_VOC_ANNOTATION_OBJ) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_00212fb1f9d3bf1c, []int{0, 0}
}

type PK_PASCAL_VOC_ANNOTATION_CMD_PASCAL_VOC_ANNOTATION int32

const (
	PK_PASCAL_VOC_ANNOTATION_UNKNOWN PK_PASCAL_VOC_ANNOTATION_CMD_PASCAL_VOC_ANNOTATION = 0
	PK_PASCAL_VOC_ANNOTATION_CMD     PK_PASCAL_VOC_ANNOTATION_CMD_PASCAL_VOC_ANNOTATION = 168493057
)

var PK_PASCAL_VOC_ANNOTATION_CMD_PASCAL_VOC_ANNOTATION_name = map[int32]string{
	0:         "UNKNOWN",
	168493057: "CMD",
}

var PK_PASCAL_VOC_ANNOTATION_CMD_PASCAL_VOC_ANNOTATION_value = map[string]int32{
	"UNKNOWN": 0,
	"CMD":     168493057,
}

func (x PK_PASCAL_VOC_ANNOTATION_CMD_PASCAL_VOC_ANNOTATION) String() string {
	return proto.EnumName(PK_PASCAL_VOC_ANNOTATION_CMD_PASCAL_VOC_ANNOTATION_name, int32(x))
}

func (PK_PASCAL_VOC_ANNOTATION_CMD_PASCAL_VOC_ANNOTATION) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_00212fb1f9d3bf1c, []int{1, 0}
}

type PK_PASCAL_VOC_SAVE_REQ_CMD_PASCAL_VOC_SAVE_REQ int32

const (
	PK_PASCAL_VOC_SAVE_REQ_UNKNOWN PK_PASCAL_VOC_SAVE_REQ_CMD_PASCAL_VOC_SAVE_REQ = 0
	PK_PASCAL_VOC_SAVE_REQ_CMD     PK_PASCAL_VOC_SAVE_REQ_CMD_PASCAL_VOC_SAVE_REQ = 168493057
)

var PK_PASCAL_VOC_SAVE_REQ_CMD_PASCAL_VOC_SAVE_REQ_name = map[int32]string{
	0:         "UNKNOWN",
	168493057: "CMD",
}

var PK_PASCAL_VOC_SAVE_REQ_CMD_PASCAL_VOC_SAVE_REQ_value = map[string]int32{
	"UNKNOWN": 0,
	"CMD":     168493057,
}

func (x PK_PASCAL_VOC_SAVE_REQ_CMD_PASCAL_VOC_SAVE_REQ) String() string {
	return proto.EnumName(PK_PASCAL_VOC_SAVE_REQ_CMD_PASCAL_VOC_SAVE_REQ_name, int32(x))
}

func (PK_PASCAL_VOC_SAVE_REQ_CMD_PASCAL_VOC_SAVE_REQ) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_00212fb1f9d3bf1c, []int{2, 0}
}

type PK_PASCAL_VOC_SAVE_RSP_CMD_PASCAL_VOC_SAVE_RSP int32

const (
	PK_PASCAL_VOC_SAVE_RSP_UNKNOWN PK_PASCAL_VOC_SAVE_RSP_CMD_PASCAL_VOC_SAVE_RSP = 0
	PK_PASCAL_VOC_SAVE_RSP_CMD     PK_PASCAL_VOC_SAVE_RSP_CMD_PASCAL_VOC_SAVE_RSP = 185204737
)

var PK_PASCAL_VOC_SAVE_RSP_CMD_PASCAL_VOC_SAVE_RSP_name = map[int32]string{
	0:         "UNKNOWN",
	185204737: "CMD",
}

var PK_PASCAL_VOC_SAVE_RSP_CMD_PASCAL_VOC_SAVE_RSP_value = map[string]int32{
	"UNKNOWN": 0,
	"CMD":     185204737,
}

func (x PK_PASCAL_VOC_SAVE_RSP_CMD_PASCAL_VOC_SAVE_RSP) String() string {
	return proto.EnumName(PK_PASCAL_VOC_SAVE_RSP_CMD_PASCAL_VOC_SAVE_RSP_name, int32(x))
}

func (PK_PASCAL_VOC_SAVE_RSP_CMD_PASCAL_VOC_SAVE_RSP) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_00212fb1f9d3bf1c, []int{3, 0}
}

type PK_PASCAL_VOC_ANNOTATION_OBJ struct {
	ObjName              string   `protobuf:"bytes,1,opt,name=objName,proto3" json:"objName,omitempty"`
	ObjPose              string   `protobuf:"bytes,2,opt,name=objPose,proto3" json:"objPose,omitempty"`
	ObjTruncated         int32    `protobuf:"varint,3,opt,name=objTruncated,proto3" json:"objTruncated,omitempty"`
	ObjDifficult         int32    `protobuf:"varint,4,opt,name=objDifficult,proto3" json:"objDifficult,omitempty"`
	BndboxXmin           int32    `protobuf:"varint,5,opt,name=bndboxXmin,proto3" json:"bndboxXmin,omitempty"`
	BndboxYmin           int32    `protobuf:"varint,6,opt,name=bndboxYmin,proto3" json:"bndboxYmin,omitempty"`
	BndboxXmax           int32    `protobuf:"varint,7,opt,name=bndboxXmax,proto3" json:"bndboxXmax,omitempty"`
	BndboxYmax           int32    `protobuf:"varint,8,opt,name=bndboxYmax,proto3" json:"bndboxYmax,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) Reset()         { *m = PK_PASCAL_VOC_ANNOTATION_OBJ{} }
func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) String() string { return proto.CompactTextString(m) }
func (*PK_PASCAL_VOC_ANNOTATION_OBJ) ProtoMessage()    {}
func (*PK_PASCAL_VOC_ANNOTATION_OBJ) Descriptor() ([]byte, []int) {
	return fileDescriptor_00212fb1f9d3bf1c, []int{0}
}

func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION_OBJ.Unmarshal(m, b)
}
func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION_OBJ.Marshal(b, m, deterministic)
}
func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) XXX_Merge(src proto.Message) {
	xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION_OBJ.Merge(m, src)
}
func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) XXX_Size() int {
	return xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION_OBJ.Size(m)
}
func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) XXX_DiscardUnknown() {
	xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION_OBJ.DiscardUnknown(m)
}

var xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION_OBJ proto.InternalMessageInfo

func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) GetObjName() string {
	if m != nil {
		return m.ObjName
	}
	return ""
}

func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) GetObjPose() string {
	if m != nil {
		return m.ObjPose
	}
	return ""
}

func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) GetObjTruncated() int32 {
	if m != nil {
		return m.ObjTruncated
	}
	return 0
}

func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) GetObjDifficult() int32 {
	if m != nil {
		return m.ObjDifficult
	}
	return 0
}

func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) GetBndboxXmin() int32 {
	if m != nil {
		return m.BndboxXmin
	}
	return 0
}

func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) GetBndboxYmin() int32 {
	if m != nil {
		return m.BndboxYmin
	}
	return 0
}

func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) GetBndboxXmax() int32 {
	if m != nil {
		return m.BndboxXmax
	}
	return 0
}

func (m *PK_PASCAL_VOC_ANNOTATION_OBJ) GetBndboxYmax() int32 {
	if m != nil {
		return m.BndboxYmax
	}
	return 0
}

type PK_PASCAL_VOC_ANNOTATION struct {
	Folder               string                          `protobuf:"bytes,1,opt,name=folder,proto3" json:"folder,omitempty"`
	Filename             string                          `protobuf:"bytes,2,opt,name=filename,proto3" json:"filename,omitempty"`
	Path                 string                          `protobuf:"bytes,3,opt,name=path,proto3" json:"path,omitempty"`
	Sourcedatabase       string                          `protobuf:"bytes,4,opt,name=sourcedatabase,proto3" json:"sourcedatabase,omitempty"`
	SzWidth              int32                           `protobuf:"varint,5,opt,name=szWidth,proto3" json:"szWidth,omitempty"`
	SzHeight             int32                           `protobuf:"varint,6,opt,name=szHeight,proto3" json:"szHeight,omitempty"`
	SzDepth              int32                           `protobuf:"varint,7,opt,name=szDepth,proto3" json:"szDepth,omitempty"`
	Segmented            int32                           `protobuf:"varint,8,opt,name=segmented,proto3" json:"segmented,omitempty"`
	Objs                 []*PK_PASCAL_VOC_ANNOTATION_OBJ `protobuf:"bytes,9,rep,name=objs,proto3" json:"objs,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                        `json:"-"`
	XXX_unrecognized     []byte                          `json:"-"`
	XXX_sizecache        int32                           `json:"-"`
}

func (m *PK_PASCAL_VOC_ANNOTATION) Reset()         { *m = PK_PASCAL_VOC_ANNOTATION{} }
func (m *PK_PASCAL_VOC_ANNOTATION) String() string { return proto.CompactTextString(m) }
func (*PK_PASCAL_VOC_ANNOTATION) ProtoMessage()    {}
func (*PK_PASCAL_VOC_ANNOTATION) Descriptor() ([]byte, []int) {
	return fileDescriptor_00212fb1f9d3bf1c, []int{1}
}

func (m *PK_PASCAL_VOC_ANNOTATION) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION.Unmarshal(m, b)
}
func (m *PK_PASCAL_VOC_ANNOTATION) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION.Marshal(b, m, deterministic)
}
func (m *PK_PASCAL_VOC_ANNOTATION) XXX_Merge(src proto.Message) {
	xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION.Merge(m, src)
}
func (m *PK_PASCAL_VOC_ANNOTATION) XXX_Size() int {
	return xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION.Size(m)
}
func (m *PK_PASCAL_VOC_ANNOTATION) XXX_DiscardUnknown() {
	xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION.DiscardUnknown(m)
}

var xxx_messageInfo_PK_PASCAL_VOC_ANNOTATION proto.InternalMessageInfo

func (m *PK_PASCAL_VOC_ANNOTATION) GetFolder() string {
	if m != nil {
		return m.Folder
	}
	return ""
}

func (m *PK_PASCAL_VOC_ANNOTATION) GetFilename() string {
	if m != nil {
		return m.Filename
	}
	return ""
}

func (m *PK_PASCAL_VOC_ANNOTATION) GetPath() string {
	if m != nil {
		return m.Path
	}
	return ""
}

func (m *PK_PASCAL_VOC_ANNOTATION) GetSourcedatabase() string {
	if m != nil {
		return m.Sourcedatabase
	}
	return ""
}

func (m *PK_PASCAL_VOC_ANNOTATION) GetSzWidth() int32 {
	if m != nil {
		return m.SzWidth
	}
	return 0
}

func (m *PK_PASCAL_VOC_ANNOTATION) GetSzHeight() int32 {
	if m != nil {
		return m.SzHeight
	}
	return 0
}

func (m *PK_PASCAL_VOC_ANNOTATION) GetSzDepth() int32 {
	if m != nil {
		return m.SzDepth
	}
	return 0
}

func (m *PK_PASCAL_VOC_ANNOTATION) GetSegmented() int32 {
	if m != nil {
		return m.Segmented
	}
	return 0
}

func (m *PK_PASCAL_VOC_ANNOTATION) GetObjs() []*PK_PASCAL_VOC_ANNOTATION_OBJ {
	if m != nil {
		return m.Objs
	}
	return nil
}

type PK_PASCAL_VOC_SAVE_REQ struct {
	Filename             string                    `protobuf:"bytes,1,opt,name=filename,proto3" json:"filename,omitempty"`
	Annotation           *PK_PASCAL_VOC_ANNOTATION `protobuf:"bytes,2,opt,name=annotation,proto3" json:"annotation,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                  `json:"-"`
	XXX_unrecognized     []byte                    `json:"-"`
	XXX_sizecache        int32                     `json:"-"`
}

func (m *PK_PASCAL_VOC_SAVE_REQ) Reset()         { *m = PK_PASCAL_VOC_SAVE_REQ{} }
func (m *PK_PASCAL_VOC_SAVE_REQ) String() string { return proto.CompactTextString(m) }
func (*PK_PASCAL_VOC_SAVE_REQ) ProtoMessage()    {}
func (*PK_PASCAL_VOC_SAVE_REQ) Descriptor() ([]byte, []int) {
	return fileDescriptor_00212fb1f9d3bf1c, []int{2}
}

func (m *PK_PASCAL_VOC_SAVE_REQ) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_PK_PASCAL_VOC_SAVE_REQ.Unmarshal(m, b)
}
func (m *PK_PASCAL_VOC_SAVE_REQ) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_PK_PASCAL_VOC_SAVE_REQ.Marshal(b, m, deterministic)
}
func (m *PK_PASCAL_VOC_SAVE_REQ) XXX_Merge(src proto.Message) {
	xxx_messageInfo_PK_PASCAL_VOC_SAVE_REQ.Merge(m, src)
}
func (m *PK_PASCAL_VOC_SAVE_REQ) XXX_Size() int {
	return xxx_messageInfo_PK_PASCAL_VOC_SAVE_REQ.Size(m)
}
func (m *PK_PASCAL_VOC_SAVE_REQ) XXX_DiscardUnknown() {
	xxx_messageInfo_PK_PASCAL_VOC_SAVE_REQ.DiscardUnknown(m)
}

var xxx_messageInfo_PK_PASCAL_VOC_SAVE_REQ proto.InternalMessageInfo

func (m *PK_PASCAL_VOC_SAVE_REQ) GetFilename() string {
	if m != nil {
		return m.Filename
	}
	return ""
}

func (m *PK_PASCAL_VOC_SAVE_REQ) GetAnnotation() *PK_PASCAL_VOC_ANNOTATION {
	if m != nil {
		return m.Annotation
	}
	return nil
}

type PK_PASCAL_VOC_SAVE_RSP struct {
	Errmsg               string   `protobuf:"bytes,1,opt,name=errmsg,proto3" json:"errmsg,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *PK_PASCAL_VOC_SAVE_RSP) Reset()         { *m = PK_PASCAL_VOC_SAVE_RSP{} }
func (m *PK_PASCAL_VOC_SAVE_RSP) String() string { return proto.CompactTextString(m) }
func (*PK_PASCAL_VOC_SAVE_RSP) ProtoMessage()    {}
func (*PK_PASCAL_VOC_SAVE_RSP) Descriptor() ([]byte, []int) {
	return fileDescriptor_00212fb1f9d3bf1c, []int{3}
}

func (m *PK_PASCAL_VOC_SAVE_RSP) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_PK_PASCAL_VOC_SAVE_RSP.Unmarshal(m, b)
}
func (m *PK_PASCAL_VOC_SAVE_RSP) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_PK_PASCAL_VOC_SAVE_RSP.Marshal(b, m, deterministic)
}
func (m *PK_PASCAL_VOC_SAVE_RSP) XXX_Merge(src proto.Message) {
	xxx_messageInfo_PK_PASCAL_VOC_SAVE_RSP.Merge(m, src)
}
func (m *PK_PASCAL_VOC_SAVE_RSP) XXX_Size() int {
	return xxx_messageInfo_PK_PASCAL_VOC_SAVE_RSP.Size(m)
}
func (m *PK_PASCAL_VOC_SAVE_RSP) XXX_DiscardUnknown() {
	xxx_messageInfo_PK_PASCAL_VOC_SAVE_RSP.DiscardUnknown(m)
}

var xxx_messageInfo_PK_PASCAL_VOC_SAVE_RSP proto.InternalMessageInfo

func (m *PK_PASCAL_VOC_SAVE_RSP) GetErrmsg() string {
	if m != nil {
		return m.Errmsg
	}
	return ""
}

func init() {
	proto.RegisterEnum("pb.PK_PASCAL_VOC_ANNOTATION_OBJ_CMD_PASCAL_VOC_ANNOTATION_OBJ", PK_PASCAL_VOC_ANNOTATION_OBJ_CMD_PASCAL_VOC_ANNOTATION_OBJ_name, PK_PASCAL_VOC_ANNOTATION_OBJ_CMD_PASCAL_VOC_ANNOTATION_OBJ_value)
	proto.RegisterEnum("pb.PK_PASCAL_VOC_ANNOTATION_CMD_PASCAL_VOC_ANNOTATION", PK_PASCAL_VOC_ANNOTATION_CMD_PASCAL_VOC_ANNOTATION_name, PK_PASCAL_VOC_ANNOTATION_CMD_PASCAL_VOC_ANNOTATION_value)
	proto.RegisterEnum("pb.PK_PASCAL_VOC_SAVE_REQ_CMD_PASCAL_VOC_SAVE_REQ", PK_PASCAL_VOC_SAVE_REQ_CMD_PASCAL_VOC_SAVE_REQ_name, PK_PASCAL_VOC_SAVE_REQ_CMD_PASCAL_VOC_SAVE_REQ_value)
	proto.RegisterEnum("pb.PK_PASCAL_VOC_SAVE_RSP_CMD_PASCAL_VOC_SAVE_RSP", PK_PASCAL_VOC_SAVE_RSP_CMD_PASCAL_VOC_SAVE_RSP_name, PK_PASCAL_VOC_SAVE_RSP_CMD_PASCAL_VOC_SAVE_RSP_value)
	proto.RegisterType((*PK_PASCAL_VOC_ANNOTATION_OBJ)(nil), "pb.PK_PASCAL_VOC_ANNOTATION_OBJ")
	proto.RegisterType((*PK_PASCAL_VOC_ANNOTATION)(nil), "pb.PK_PASCAL_VOC_ANNOTATION")
	proto.RegisterType((*PK_PASCAL_VOC_SAVE_REQ)(nil), "pb.PK_PASCAL_VOC_SAVE_REQ")
	proto.RegisterType((*PK_PASCAL_VOC_SAVE_RSP)(nil), "pb.PK_PASCAL_VOC_SAVE_RSP")
}

func init() { proto.RegisterFile("api.proto", fileDescriptor_00212fb1f9d3bf1c) }

var fileDescriptor_00212fb1f9d3bf1c = []byte{
	// 465 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x7c, 0x93, 0x41, 0x8f, 0xd2, 0x40,
	0x14, 0xc7, 0x2d, 0xb0, 0xb0, 0x7d, 0x18, 0xb3, 0x99, 0xc3, 0x3a, 0x1a, 0x34, 0xa4, 0x07, 0xc3,
	0x89, 0x03, 0xee, 0xc1, 0x83, 0x97, 0x0a, 0x9b, 0xa8, 0xeb, 0xb6, 0xb5, 0xe0, 0x2e, 0x7b, 0x6a,
	0x66, 0xe8, 0x00, 0x25, 0x74, 0xa6, 0xe9, 0x0c, 0x49, 0xb3, 0x27, 0xfd, 0x32, 0xc6, 0x83, 0x1f,
	0xc7, 0x8f, 0xe0, 0x07, 0x31, 0x2d, 0xa5, 0x50, 0x62, 0xb9, 0xf1, 0x7f, 0xff, 0xff, 0xe3, 0xbd,
	0xfc, 0xe6, 0x15, 0x74, 0x12, 0x05, 0xfd, 0x28, 0x16, 0x4a, 0xa0, 0x5a, 0x44, 0x8d, 0x3f, 0x35,
	0xe8, 0x38, 0x37, 0x9e, 0x63, 0x8e, 0x87, 0xe6, 0x17, 0xef, 0xce, 0x1e, 0x7a, 0xa6, 0x65, 0xd9,
	0x13, 0x73, 0xf2, 0xc9, 0xb6, 0x3c, 0xfb, 0xc3, 0x67, 0x84, 0xa1, 0x25, 0xe8, 0xca, 0x22, 0x21,
	0xc3, 0x5a, 0x57, 0xeb, 0xe9, 0xee, 0x4e, 0xe6, 0x8e, 0x23, 0x24, 0xc3, 0xb5, 0xc2, 0x49, 0x25,
	0x32, 0xe0, 0xa9, 0xa0, 0xab, 0x49, 0xbc, 0xe1, 0x33, 0xa2, 0x98, 0x8f, 0xeb, 0x5d, 0xad, 0x77,
	0xe6, 0x96, 0x6a, 0x79, 0x66, 0x14, 0xcc, 0xe7, 0xc1, 0x6c, 0xb3, 0x56, 0xb8, 0x51, 0x64, 0x8a,
	0x1a, 0x7a, 0x0d, 0x40, 0xb9, 0x4f, 0x45, 0x32, 0x0d, 0x03, 0x8e, 0xcf, 0xb2, 0xc4, 0x41, 0x65,
	0xef, 0x3f, 0xa4, 0x7e, 0xf3, 0xd0, 0x7f, 0x28, 0xf9, 0xd3, 0x90, 0x24, 0xb8, 0x55, 0xee, 0x27,
	0xc9, 0x61, 0x3f, 0x49, 0xf0, 0x79, 0xb9, 0x9f, 0x24, 0xc6, 0x3b, 0x78, 0x35, 0xbc, 0x1d, 0x9d,
	0x80, 0xd3, 0x86, 0xd6, 0x37, 0xeb, 0xc6, 0xb2, 0xef, 0xad, 0x8b, 0x27, 0x08, 0xa0, 0x3e, 0xbc,
	0x1d, 0x5d, 0xfc, 0xf8, 0xfe, 0xdb, 0x31, 0xfe, 0xd6, 0x00, 0x57, 0x61, 0x45, 0x97, 0xd0, 0x9c,
	0x8b, 0xb5, 0xcf, 0xe2, 0x9c, 0x68, 0xae, 0xd0, 0x4b, 0x38, 0x9f, 0x07, 0x6b, 0xc6, 0x53, 0xd6,
	0x5b, 0xa2, 0x85, 0x46, 0x08, 0x1a, 0x11, 0x51, 0xcb, 0x0c, 0xa5, 0xee, 0x66, 0xbf, 0xd1, 0x1b,
	0x78, 0x26, 0xc5, 0x26, 0x9e, 0x31, 0x9f, 0x28, 0x42, 0x89, 0x64, 0x19, 0x44, 0xdd, 0x3d, 0xaa,
	0xa6, 0x0f, 0x25, 0x1f, 0xef, 0x03, 0x5f, 0x2d, 0x73, 0x86, 0x3b, 0x99, 0x4e, 0x94, 0x8f, 0x1f,
	0x59, 0xb0, 0x58, 0xaa, 0x1c, 0x5f, 0xa1, 0xb7, 0x5d, 0x23, 0x16, 0xa9, 0x65, 0x4e, 0x6e, 0x27,
	0x51, 0x07, 0x74, 0xc9, 0x16, 0x21, 0xe3, 0xe9, 0xdb, 0x6e, 0xa9, 0xed, 0x0b, 0xe8, 0x0a, 0x1a,
	0x82, 0xae, 0x24, 0xd6, 0xbb, 0xf5, 0x5e, 0x7b, 0xd0, 0xed, 0x47, 0xb4, 0x7f, 0xea, 0xc0, 0xdc,
	0x2c, 0x6d, 0x5c, 0xc1, 0x8b, 0x4a, 0xd4, 0xd5, 0x98, 0x7f, 0x6a, 0x70, 0x59, 0xfe, 0xf3, 0xb1,
	0x79, 0x77, 0xed, 0xb9, 0xd7, 0x5f, 0x4b, 0x30, 0xb5, 0x23, 0x98, 0xef, 0x01, 0x08, 0xe7, 0x42,
	0x11, 0x15, 0x08, 0x9e, 0xa1, 0x6e, 0x0f, 0x3a, 0xa7, 0x16, 0x75, 0x0f, 0xf2, 0xc6, 0x00, 0x9e,
	0x1f, 0xad, 0x5a, 0x0c, 0xad, 0x5c, 0xd4, 0xff, 0xff, 0x9e, 0x63, 0x27, 0x3d, 0x06, 0x16, 0xc7,
	0xa1, 0x5c, 0xec, 0x8e, 0x61, 0xab, 0x2a, 0xa7, 0x8c, 0x9d, 0x8a, 0x29, 0xbf, 0xa6, 0xb4, 0x99,
	0x7d, 0xd7, 0x6f, 0xff, 0x05, 0x00, 0x00, 0xff, 0xff, 0x5c, 0xd1, 0x5f, 0x33, 0xe4, 0x03, 0x00,
	0x00,
}
