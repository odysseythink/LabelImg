package main

/*
#include "c_2_go_interface.h"
void C_Callback(int cmd,void* p,int l);;
*/
import "C"
import (
	"fmt"
	"unsafe"

	"demo.com/LabelImg/go_label_img/pb"
	"github.com/golang/protobuf/proto"
	"mlib.com/mlog"
)

// go build -buildmode=c-archive -o liblabelImg.a
func parseReqFromCPointer(req unsafe.Pointer, reqlen C.int, pbmsg proto.Message) error {
	reqarr := unsafe.Slice((*C.char)(req), reqlen)
	reqbuf := make([]byte, reqlen)
	for iLoop := 0; iLoop < int(reqlen); iLoop++ {
		reqbuf[iLoop] = byte(reqarr[iLoop])
	}
	err := proto.Unmarshal(reqbuf, pbmsg)
	if err != nil {
		mlog.Errorf("proto.Marshal failed")
		return err
	}
	return nil
}

func packRspToCPointer(pbmsg proto.Message, l *C.int) (rsp unsafe.Pointer) {
	data, err := proto.Marshal(pbmsg)
	if err != nil {
		mlog.Errorf("proto.Marshal failed")
		*l = C.int(0)
		return nil
	}

	p := C.malloc(C.size_t(len(data)))
	arr := unsafe.Slice((*C.char)(p), len(data))
	for k, v := range data {
		arr[k] = C.char(v)
	}
	*l = C.int(len(data))
	return p
}

func callNoticeCallback(cmd uint32, pbmsg proto.Message) {
	var l C.int = 0
	data, err := proto.Marshal(pbmsg)
	if err != nil {
		mlog.Errorf("proto.Marshal failed")
		return
	}

	p := C.malloc(C.size_t(len(data)))
	arr := unsafe.Slice((*C.char)(p), len(data))
	for k, v := range data {
		arr[k] = C.char(v)
	}
	l = C.int(len(data))
	C.C_Callback(C.int(cmd), p, l)
}

//export PascalVocSave
func PascalVocSave(req unsafe.Pointer, reqlen C.int, l *C.int) unsafe.Pointer {
	myreq := pb.PK_PASCAL_VOC_SAVE_REQ{}
	myrsp := pb.PK_PASCAL_VOC_SAVE_RSP{}
	err := parseReqFromCPointer(req, reqlen, &myreq)
	if err != nil {
		mlog.Errorf("parse req failed:%v", err)
		myrsp.Errmsg = fmt.Sprintf("parse req failed:%v", err)
		return packRspToCPointer(&myrsp, l)
	}
	if myreq.Filename == "" {
		mlog.Errorf("invalid arg")
		myrsp.Errmsg = "invalid arg"
		return packRspToCPointer(&myrsp, l)
	}
	annotation := &PascalVocAnnotation{
		Folder:   myreq.Annotation.Folder,
		Filename: myreq.Annotation.Filename,
		Path:     myreq.Annotation.Path,
		Source: PascalVocAnnotationSource{
			Database: myreq.Annotation.Sourcedatabase,
		},
		Size: PascalVocAnnotationSize{
			Width:  int(myreq.Annotation.SzWidth),
			Height: int(myreq.Annotation.SzHeight),
			Depth:  int(myreq.Annotation.SzDepth),
		},
		Segmented: int(myreq.Annotation.Segmented),
	}
	for _, v := range myreq.Annotation.Objs {
		if annotation.Objs == nil {
			annotation.Objs = make([]PascalVocAnnotationObject, 0)
		}
		annotation.Objs = append(annotation.Objs, PascalVocAnnotationObject{
			Name:      v.ObjName,
			Pose:      v.ObjPose,
			Truncated: int(v.ObjTruncated),
			Difficult: int(v.ObjDifficult),
			Bndbox: PascalVocAnnotationObjectBndbox{
				Xmin: int(v.BndboxXmin),
				Ymin: int(v.BndboxYmin),
				Xmax: int(v.BndboxXmax),
				Ymax: int(v.BndboxYmax),
			},
		})
	}
	myrsp.Errmsg = fmt.Sprintf("%v", annotation.Write(myreq.Filename))

	return packRspToCPointer(&myrsp, l)
}

func main() {
}
