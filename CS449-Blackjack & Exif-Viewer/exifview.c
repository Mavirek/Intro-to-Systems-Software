//Allen Poon
//CS449 
//Project 1 Part 2 Exif Viewer

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  struct exifHeader{
	  unsigned short fileStartMarker;
	  unsigned short appMarker;
	  unsigned short appLength;
	  unsigned char exifString[4];
	  unsigned short nullZeros;
	  unsigned char endian[2];
	  unsigned short version;
	  unsigned int offset;
  }header; 
  struct tiffTag{
	  unsigned short identifier;
	  unsigned short datatype;
	  unsigned int numItems;
	  unsigned int offset;
  }tag;

  FILE *f;
  f = fopen(argv[1], "rb");
  if(f==NULL) //verifies file input
  {
	  printf("Invalid file\n");
	  return 0;
  }
  else if(argc>2 || argc<2) //verifies the input has the correct number of inputs
  {
	  printf("Invalid number of arguments\n");
	  return 0;
  }
  if(fread(&header, sizeof(header), 1, f)==1) //reads in the first 20 bytes for the header
  {
	  //printf("app = %x\n",header.appMarker);
	  if(header.appMarker!=0xE1FF) //verifies image is APP1 type
	  {
		  printf("Wrong APP type. Must be APP1 type\n");
		  return 0;
	  }
	  //printf("%s\n",header.exifString);
	  if(strcmp(header.exifString,"Exif")!=0) //verifies EXIF
	  {
		  printf("Tag was not found\n");
		  return 0;
	  }
	  //printf("%s\n",header.endian);
	  if(strncmp(header.endian,"II",2)!=0) //verifies version number
	  {
		  printf("Endian not supported");
		  return 0;
	  }
  }
  
  unsigned short count;
  fread(&count, sizeof(unsigned short), 1, f);
  //printf("count = %d\n",count);
  
  int filePos=0;
  char manufact[30];
  char camModel[30];
	
  for(int x=0;x<count;x++)
  {
	  fread(&tag, sizeof(tag), 1, f);
	  //printf("identifier = %x\n",tag.identifier);
	  
	  if(tag.identifier==0x010F) //checks if tag ID is for manufacturer
	  {
		  filePos=ftell(f); //save original file position
		  fseek(f,tag.offset+12,SEEK_SET); //goto offset and then read data items
		  //printf("num items = %d\n",tag.numItems);
		  fread(&manufact,sizeof(char),tag.numItems,f);
		  printf("Manufacturer: %s\n",manufact);
		  fseek(f,filePos,SEEK_SET); //return to original position in file
	  }
	  else if(tag.identifier==0x0110) //checks tag ID for camera model
	  {
		  filePos=ftell(f);
		  fseek(f,tag.offset+12,SEEK_SET);
		  fread(&camModel,sizeof(char),tag.numItems,f);
		  printf("Camera Model: %s\n",camModel);
		  fseek(f,filePos,SEEK_SET);
	  }
	  else if(tag.identifier==0x8769) //checks for EXIF sub block address
	  {
		  fseek(f,tag.offset+12,SEEK_SET); //goto offset specified in EXIF sub block tag
		  break;
	  }
	  
  }
  unsigned short count2=0;
  struct tiffTag tag2;
  unsigned int j,k;
  fread(&count2, sizeof(unsigned short), 1, f);
  for(int x=0;x<count2;x++)
  {
	  fread(&tag2,sizeof(tag2),1,f);
	  if(tag2.identifier==0xA002) //checks for width
	  {
		  printf("Width: %d pixels\n",tag2.offset);
	  }
	  else if(tag2.identifier==0xA003) //checks for height
	  {
		  printf("Height: %d pixels\n",tag2.offset);
	  }
	  else if(tag2.identifier==0x8827) //checks for ISO speed
	  {
		  printf("ISO: ISO %d\n",tag2.offset);
	  }
	  else if(tag2.identifier==0x829a) //checks for exposure speed
	  {
		  //data type 5 procedure
		  filePos=ftell(f); //save original file position
		  fseek(f,tag2.offset+12,SEEK_SET);
		  fread(&j,sizeof(unsigned int),1,f); //read first unsigned int
		  fread(&k,sizeof(unsigned int),1,f); //read second unsigned int
		  fseek(f,filePos,SEEK_SET); //go back to original position
		  printf("Exposure Time: %d/%d second\n",j,k);
	  }
	  else if(tag2.identifier==0x829d) //checks for F-stop
	  {
		  filePos=ftell(f);
		  fseek(f,tag2.offset+12,SEEK_SET);
		  fread(&j,sizeof(unsigned int),1,f);
		  fread(&k,sizeof(unsigned int),1,f);
		  fseek(f,filePos,SEEK_SET);
		  printf("F-Stop: f/%.1f\n",(double)j/k); //display ratio of the two unsigned ints
	  }
	  else if(tag2.identifier==0x920A) //checks for focal length
	  {
		  filePos=ftell(f);
		  fseek(f,tag2.offset+12,SEEK_SET);
		  fread(&j,sizeof(unsigned int),1,f);
		  fread(&k,sizeof(unsigned int),1,f);
		  fseek(f,filePos,SEEK_SET);
		 // printf("J = %d  K = %d\n",j,k);
		  printf("Focal Length: %.0f mm\n",(double)j/k);
	  }
	  else if(tag2.identifier==0x9003) //checks for date taken
	  {
		  char date[30];
		  filePos=ftell(f);
		  fseek(f,tag2.offset+12,SEEK_SET);
		  fread(&date,sizeof(char),tag2.numItems,f);
		  printf("Date Taken: %s\n",date);
		  fseek(f,filePos,SEEK_SET);
	  }
  }
  fclose(f); //close file
  return 0;
}
