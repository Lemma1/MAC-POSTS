#include "path.h"

MNM_Path::MNM_Path()
{
	m_path_vec = TVec<TInt>();
};

MNM_Path::~MNM_Path()
{
	m_path_vec.Clr(false);
};


int MNM_Path::AddLinkID(TInt linkID)
{
	if (m_path_vec.Add(linkID) > 0){
	 return 0;
	}
	else {
	 return -1;
	}
};

TInt* MNM_Path::BegLinkID() 
{
  return m_path_vec.BegI();
};

TInt* MNM_Path::EndLinkID() 
{
  return m_path_vec.EndI();
};

int MNM_Path::GetSize()
{
  return m_path_vec.Len();
};
