/*1:*/
#line 48 "./pyramid_prod.hweb"

#ifndef PYRAMID_PROD_H
#define PYRAMID_PROD_H

#include "int_sequence.h"
#include "rfs_tensor.h"
#include "gs_tensor.h"
#include "t_container.h"

#include <vector> 

using namespace std;

/*2:*/
#line 71 "./pyramid_prod.hweb"

class USubTensor:public URTensor{
public:
USubTensor(const TensorDimens&bdims,const TensorDimens&hdims,
const FGSContainer&cont,const vector<IntSequence> &lst);
void addKronColumn(int i,const vector<const FGSTensor*> &ts,
const IntSequence&pindex);
};

/*:2*/
#line 61 "./pyramid_prod.hweb"
;

#endif

/*:1*/
