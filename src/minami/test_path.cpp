#include "Snap.h"

#include "path.h"

int main(int argc, char **argv)
{
  MNM_Path p = MNM_Path();
  for (TInt i=0; i< 10; i++){
    p.AddLinkID(i);
    printf("adding %d\n", (int)i);
  }
  printf("The size is now %d\n", p.GetSize());

  for (TInt* eI = p.BegLinkID(); eI != p.EndLinkID(); eI++) {
    printf("Now is Link ID %d\n", (int) *eI);
  }

}