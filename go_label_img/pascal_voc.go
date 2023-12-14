package main

import (
	"encoding/xml"
	"fmt"
	"log"
	"os"
)

type PascalVocAnnotationSource struct {
	XMLName  xml.Name `xml:"source"`
	Database string   `xml:"database"`
}

type PascalVocAnnotationSize struct {
	XMLName xml.Name `xml:"size"`
	Width   int      `xml:"width"`
	Height  int      `xml:"height"`
	Depth   int      `xml:"depth"`
}

type PascalVocAnnotationObjectBndbox struct {
	XMLName xml.Name `xml:"bndbox"`
	Xmin    int      `xml:"xmin"`
	Ymin    int      `xml:"ymin"`
	Xmax    int      `xml:"xmax"`
	Ymax    int      `xml:"ymax"`
}

type PascalVocAnnotationObject struct {
	XMLName   xml.Name `xml:"object"`
	Name      string   `xml:"name"`
	Pose      string   `xml:"pose"`
	Truncated int      `xml:"truncated"`
	Difficult int      `xml:"difficult"`

	Bndbox PascalVocAnnotationObjectBndbox `xml:"bndbox"`
}

type PascalVocAnnotation struct {
	XMLName  xml.Name `xml:"annotation"`
	Folder   string   `xml:"folder"`
	Filename string   `xml:"filename"`
	Path     string   `xml:"path"`

	Source PascalVocAnnotationSource `xml:"source"`

	Size      PascalVocAnnotationSize `xml:"size"`
	Segmented int                     `xml:"segmented"`

	Objs []PascalVocAnnotationObject `xml:"object"`
}

func (a *PascalVocAnnotation) Write(filename string) error {
	bintmp, err := xml.Marshal(a)
	if err != nil {
		log.Printf("[PascalVocAnnotation write] xml.Marshal(%#v) failed:%v\n", a, err)
		return fmt.Errorf("[PascalVocAnnotation write] xml.Marshal(%#v) failed:%v", a, err)
	}
	os.WriteFile(filename, bintmp, 0644)
	return nil
}
