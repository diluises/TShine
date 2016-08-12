/*
 * DataContainer.cc
 *
 *  Created on: May 7, 2015
 *      Author: Silvestro di Luise
 *				Silvestro.Di.Luise@cern.ch
 *				
 */

#include <iostream>

#include "DataContainer.h"

#include "StringUtils.h"
#include "MessageMgr.h"


namespace EDM {

DataContainer::DataContainer(TFolder *f)
:fFldTop(f)
{
/**
 *
 */

	std::cout<<__PRETTY_FUNCTION__<<" "<<fFldTop<<" "<<f<<std::endl;

}



DataContainer::DataContainer(TString name, TString title)
{
/**
 *
 */


	fFldTop = new TFolder(name,title);

	// Check this ....
	fFldTop->SetOwner(kTRUE);
}


DataContainer::~DataContainer() {
/**
 *
 */

	//TODO
	//Clear or Delete according to ownership
	//
	//std::cout<<__PRETTY_FUNCTION__<<" top fld: "<<fFldTop<<std::endl;


	//if(fFldTop) std::cout<<" "<<fFldTop->GetName()<<std::endl;
}


TFolder* DataContainer::BuildFolder(TString path)
{
	/**
	 *
	 *
	 */

  if ( path.IsWhitespace() ){

	  MSG::ERROR(__PRETTY_FUNCTION__," Path is empty");
	  return 0;
  }

  TFolder* fld = 0;

  fld = GetFolder(path);

  if (fld!=0) return fld;

  TString path_pre;
  TString dir_name;

  //STR_UTILS::GetPath(path,&path_pre,&dir_name);

  std::vector<TString> list = STR_UTILS::GetPath2(path);

  if(list.size()!=2){
	  MSG::ERROR(__FUNCTION__," GetPath Failed: ", path);
	  return 0;
  }else{

	  path_pre = list[0];
	  dir_name = list[1];
  }

  // Find the mother folder and add the daughter
  fld = GetFolder(path_pre);
  if (fld==0) return 0;

  fld->SetOwner(kTRUE);

  TFolder* fld_new = fld->AddFolder(dir_name, dir_name, 0);

  fld_new->SetOwner(kTRUE);

  return fld_new;

}


TFolder* DataContainer::BuildFolders(TString path)
{
	/**
	 *
	 *
	 */



}


void DataContainer::ClearArrays(const TFolder *f) {
	/**
	 *
	 *
	 */

	//cout<<__FUNCTION__<<endl;

	if ( f==0 ) f = fFldTop;

	TCollection* folders = f->GetListOfFolders();

	TIterator*   iter    = folders->MakeIterator();
	TObject*      obj  = 0;
	TFolder*      fobj = 0;
	TClonesArray* carr = 0;
	TObjArray*    oarr = 0;

	while ( (obj=iter->Next())!=0) {
		fobj = dynamic_cast<TFolder*>(obj);
		carr = dynamic_cast<TClonesArray*>(obj);
	    oarr = dynamic_cast<TObjArray*>(obj);
	    if (fobj) this->ClearArrays(fobj);
	    if (carr) carr->Delete();
	    if (oarr) oarr->Delete();
	}

	delete iter;

	//cout<<__FUNCTION__<<" end "<<endl;
}


TClonesArray* DataContainer::GetClonesArray(TString path) const {
	/**
	 *
	 *
	 */

	TString array_path;
	TString array_name;

	//STR_UTILS::GetPath(path,&array_path, &array_name);

	std::vector<TString> list = STR_UTILS::GetPath2(path);

	if(list.size()!=2){
		  MSG::ERROR(__FUNCTION__," GetPath Failed: ", path);
		  return 0;
	}else{

		  array_path = list[0];
		  array_name = list[1];
	}

	//cout<<__FUNCTION__<<" "<<path<<" "<<array_path<<" "<<array_name<<endl;

	TFolder* f = GetFolder(array_path);

	//cout<<f<<endl;

	if ( f==0 ) return 0;

	TObject* obj = f->FindObjectAny(array_name);

	TClonesArray* array = dynamic_cast<TClonesArray*>(obj);

	return array;

}


TObjArray* DataContainer::GetObjArray(TString path) const {
	/**
	 *
	 *
	 */

	TString array_path;
	TString array_name;

	//STR_UTILS::GetPath(path,&array_path, &array_name);


	std::vector<TString> list = STR_UTILS::GetPath2(path);

	if(list.size()!=2){
		MSG::ERROR(__FUNCTION__," GetPath Failed: ", path);
		return 0;
	 }else{

		 array_path = list[0];
		 array_name = list[1];
	  }

	TFolder* f = GetFolder(array_path);
	if ( f==0 ) return 0;

	TObject* obj = f->FindObjectAny(array_name);

	TObjArray* array = dynamic_cast<TObjArray*>(obj);

	return array;

}



TFolder* DataContainer::GetFolder(TString path) const {
	/**
	 *
	 *
	 */

  if( path.IsWhitespace() || path == "." || path == "/"){
	  return fFldTop;
  }


  TString path_to;
  TString fld_name;

  //STR_UTILS::GetPath(path,&path_to,&fld_name);

  std::vector<TString> list = STR_UTILS::GetPath2(path);

  if(list.size()!=2){
  	  MSG::ERROR(__FUNCTION__," GetPath Failed: ", path);
  	  return 0;
    }else{

  	  path_to = list[0];
  	  fld_name = list[1];
    }

  TFolder* f1 = GetFolder(path_to);
  if ( f1==0 ) return 0;

  TFolder* f2 = dynamic_cast<TFolder*>(f1->FindObjectAny(fld_name));
  return f2;

}


void DataContainer::Place(TObject *obj, TString path, int opt) {
/**
 *
 *
 */
	if(obj==0) {
		MSG::ERROR(__PRETTY_FUNCTION__," null ptr to obj ");
		return;
	}

	TFolder *f = (TFolder*)GetFolder(path);

	if(f==0){
		MSG::ERROR(__PRETTY_FUNCTION__," folder ",path," not found ");
		return;
	}

	f->Add(obj);

}


void DataContainer::Print(TString opt) {
/**
 *
 *
 */

	PrintFolder(*fFldTop,1);

}





void DataContainer::PrintFolder(const TFolder& f, int lvl) {
	/**
	 *
	 */


	TString pad="*";

	for (int i=0; i<lvl-1; ++i) pad += "  ";
	if (lvl>0) pad += "|-";
	//pad += "> ";

	TCollection* folders = dynamic_cast<TCollection*>(f.GetListOfFolders());

	//std::cout<<" folders "<<f.GetName()<<" "<<folders<<std::endl;

	TIterator* iter = folders->MakeIterator();


	TObject*   obj  = 0;
	TObject*   oobj  = 0;
	TFolder*   fobj = 0;
	TObjArray* aobj = 0;

	while ( (obj=iter->Next())!=0) {

		fobj = dynamic_cast<TFolder*>(obj);
		aobj = dynamic_cast<TObjArray*>(obj);
		oobj  = dynamic_cast<TObject*>(obj);

		if(oobj && !fobj && !aobj ){
			//std::cout<< pad <<" "<<oobj->GetName()<<" \n";
		}

	    if (aobj) {
	      std::cout << pad <<"> "<<
	      obj->GetName() << "[" << aobj->GetEntries() << "]\n";

	      // if(aobj->GetEntries()<=10){
	      //  for(int i=0;i<aobj->GetEntries();++i){
	      //	 TNamed *o = (TNamed*)(*aobj)[i];
	      //	 std::cout<<pad<<" -"<<i<<" "<<o->GetName()<<endl;
	      //	 }
	      // }

	    }


	    if (fobj) {
	    	TString pad2 = pad;
	    	pad2+="[ ";
	    	pad2 += fobj->GetName();
	    	std::cout << pad2 <</* " level: "<<lvl<<*/"\n";
	    	DataContainer::PrintFolder(*fobj, lvl+1);
	    }

	}
	  delete iter;


}


std::vector<TString> DataContainer::GetPathOfFolders() {

	std::vector<TString> list;

	GetPathOfFolders(*fFldTop,"/",1,list);

	return list;
}


void DataContainer::GetPathOfFolders(const TFolder &f, TString path, int lvl, std::vector<TString>& list) {
/**
 *
 */


	TString full_path = "";

	if(lvl>1) full_path = path;

	TCollection* folders = dynamic_cast<TCollection*>(f.GetListOfFolders());


	TIterator* iter = folders->MakeIterator();

	TObject*   obj  = 0;
	TFolder*   fobj = 0;

	while ( (obj=iter->Next())!=0 ) {

		fobj = dynamic_cast<TFolder*>(obj);

		if (fobj) {

			TString level_path = full_path;

			if(lvl>1){
				level_path += "/";
				level_path += fobj->GetName();
				std::cout << level_path << std::endl;

				list.push_back(level_path);

				DataContainer::GetPathOfFolders(*fobj,level_path,lvl+1,list);

			}

			if(lvl==1){
				full_path = "./";
				full_path += obj->GetName();
				std::cout << full_path << std::endl;

				list.push_back(full_path);

				DataContainer::GetPathOfFolders(*fobj,full_path,lvl+1,list);
			}



		}

	}

	delete iter;

}



TClonesArray* DataContainer::NewClonesArray(const TString path, const TString class_name) {
/**
 *
 */

	TFolder* f = GetFolder(path);

	if ( f==0 ) return 0;

	TClonesArray* ca = new TClonesArray(class_name);
	f->Add(ca);

	cout<<"NewClonesArray "<<path<<" "<<class_name<<" "<<ca<<endl;

	return ca;

}

TObjArray*  DataContainer::NewObjArray(const TString path, const TString class_name) {
	/**
	 *
	 *
	 */

	 TFolder* f = GetFolder(path);

	 if ( f==0 ) return 0;

	 TString name = class_name;
	 name += "s";

	 TObjArray* oa = new TObjArray(100,0);

	 oa->SetOwner();
	 oa->SetName(name);

	 f->Add(oa);

	 return oa;

}


} /* namespace EDM */
