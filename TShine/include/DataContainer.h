/*
 * DataContainer.h
 *
 *  Created on: May, 2015
 *      Author: Silvestro di Luise
 *				Silvestro.Di.Luise@cern.ch
 *				
 */

#ifndef DATACONTAINER_H_
#define DATACONTAINER_H_

#include <assert.h>
#include <cassert>
#include <iostream>


#include <vector>

#include <TString.h>
#include <TClonesArray.h>
#include <TObjArray.h>
#include <TFolder.h>


#include "MessageMgr.h"


namespace EDM {

class DataContainer {

public:

	DataContainer(TFolder *);
	DataContainer(TString name, TString title="");
	virtual ~DataContainer();

	template<class T> int Get(TString path, std::vector<const T*>& v) const;
	template<class T> T* Put(const T& obj, TString path, int opt=0);

	void Place(TObject *obj, TString path, int opt=0);

	TFolder* BuildFolder(TString);
	TFolder* BuildFolders(TString);
	void ClearArrays(const TFolder* =0);

	TClonesArray* GetClonesArray(TString path) const;
	TFolder* GetFolder(TString) const;
	void GetPathOfFolders(const TFolder &, TString path, int depth, std::vector<TString>&);
	std::vector<TString> GetPathOfFolders();

	TObjArray* GetObjArray(TString path) const;

	void Print(TString opt="");
	void PrintFolder(const TFolder&, int depth);

	void Write() {if(fFldTop) fFldTop->Write();}

private:

	TClonesArray* NewClonesArray(const TString path, const TString class_name);
	TObjArray*    NewObjArray(const TString path, const TString class_name);

	TFolder *fFldTop;



};


} /* namespace EDM */



template<class T> int EDM::DataContainer::Get(TString path, std::vector<const T*>& v) const {
/**
 *
 *
 */

 std::cout<<__FUNCTION__<<std::endl;

 static TString path_new;

 TObjArray *oa = 0;

 if( oa==0 ) oa = this->GetClonesArray(path);
 if( oa==0 ) oa = this->GetObjArray(path);

 if( oa==0 ){

	 T tmpItem;
	 path_new = path;
	 path_new += "/";
	 path_new += tmpItem.ClassName();
	 path_new += "s";

	 if( oa==0 ) oa = this->GetClonesArray(path_new);
	 if( oa==0 ) oa = this->GetObjArray(path_new);

 }

 if( oa==0 ){

	 MSG::WARNING(__PRETTY_FUNCTION__," collection ", path, " not found ");
	 //exit(0);

	 return 0;
 }


 const T* obj = 0;

 int nObj = oa->GetEntries();

 int ntot_size = nObj+v.size();

 if(ntot_size>v.capacity()) v.reserve(ntot_size);

 for( int i=0; i<nObj; ++i){

	 obj = dynamic_cast<const T*>(oa->At(i));


	 if( obj==0 ){
		 MSG::ERROR(__PRETTY_FUNCTION__," array ",oa->GetName()," does not contain object type");
		 //exit(0);

		 assert(oa);

		 return 0;
	 }

	 v.push_back(obj);
 }

 std::cout<<__FUNCTION__<<" end "<<nObj<<std::endl;

 return nObj;
}


template<class T> T* EDM::DataContainer::Put(const T& obj, TString path, int opt)
{
/**
 *
 */

	TString collectName;
	static TString fullPath;


	collectName = obj.ClassName();
	fullPath = path;
	fullPath += "/";
	fullPath += collectName;
	fullPath += "s";
	//if(path=="mc") fullPath +="t";

	cout<<"full path: "<<fullPath<<" path: "<<path<<" coll: "<<collectName<<" obj: "<<&obj<<endl;


	if( opt == 0 ){

		cout<<"GetClonesArray "<<endl;

		TClonesArray *ca = GetClonesArray(fullPath);

		cout<<" ca "<<ca<<endl;

		if( ca==0 ){

			ca = NewClonesArray(path,collectName);

			//cout<<"new "<<ca<<endl;
		}

		if( ca==0 ){
			MSG::ERROR(__PRETTY_FUNCTION__," folder ",path," not found");
			exit(0);
		}


		int idx = ca->GetLast()+1;

		cout<<"put "<<idx<<endl;

		T* t = new ((*ca)[idx]) T(obj);

		cout<<" put "<<t<<endl;
		return t;

	}//opt

	if( opt == 1){

		TObjArray* oa = GetObjArray(fullPath);

		if( !oa ){
			oa = NewObjArray(path,collectName);
		}

		if( !oa ){
			MSG::ERROR(__PRETTY_FUNCTION__," folder ",path," not found");
			exit(0);
		}


		 T* t = new T(obj);

		 oa->AddLast(t);

		 return t;

	}//opt


	return 0;
}

#endif /* DATACONTAINER_H_ */
