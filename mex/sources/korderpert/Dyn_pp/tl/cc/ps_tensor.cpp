/*1:*/
#line 5 "./ps_tensor.cweb"

#include "ps_tensor.h"
#include "fs_tensor.h"
#include "tl_exception.h"
#include "tl_static.h"
#include "stack_container.h"

/*2:*/
#line 38 "./ps_tensor.cweb"

UPSTensor::fill_method UPSTensor::decideFillMethod(const FSSparseTensor&t)
{
if(t.getFillFactor()> 0.08)
return first;
else
return second;
}

/*:2*/
#line 12 "./ps_tensor.cweb"
;
/*3:*/
#line 49 "./ps_tensor.cweb"

UPSTensor::UPSTensor(const FSSparseTensor&t,const IntSequence&ss,
const IntSequence&coor,const PerTensorDimens&ptd)
:UTensor(along_col,ptd.getNVX(),
t.nrows(),ptd.calcUnfoldMaxOffset(),ptd.dimen()),
tdims(ptd)
{
TL_RAISE_IF(coor.size()!=t.dimen(),
"Wrong coordinates length of stacks for UPSTensor slicing constructor");
TL_RAISE_IF(ss.sum()!=t.nvar(),
"Wrong length of stacks for UPSTensor slicing constructor");

if(first==decideFillMethod(t))
fillFromSparseOne(t,ss,coor);
else
fillFromSparseTwo(t,ss,coor);
}


/*:3*/
#line 13 "./ps_tensor.cweb"
;
/*4:*/
#line 69 "./ps_tensor.cweb"

void UPSTensor::increment(IntSequence&v)const
{
TL_RAISE_IF(v.size()!=dimen(),
"Wrong input/output vector size in UPSTensor::increment");

UTensor::increment(v,tdims.getNVX());
}

void UPSTensor::decrement(IntSequence&v)const
{
TL_RAISE_IF(v.size()!=dimen(),
"Wrong input/output vector size in UPSTensor::decrement");

UTensor::decrement(v,tdims.getNVX());
}

/*:4*/
#line 14 "./ps_tensor.cweb"
;
/*5:*/
#line 87 "./ps_tensor.cweb"

FTensor&UPSTensor::fold()const
{
TL_RAISE("Never should come to this place in UPSTensor::fold");
FFSTensor*nothing= new FFSTensor(0,0,0);
return*nothing;
}


/*:5*/
#line 15 "./ps_tensor.cweb"
;
/*6:*/
#line 97 "./ps_tensor.cweb"

int UPSTensor::getOffset(const IntSequence&v)const
{
TL_RAISE_IF(v.size()!=dimen(),
"Wrong input vector size in UPSTensor::getOffset");

return UTensor::getOffset(v,tdims.getNVX());
}

/*:6*/
#line 16 "./ps_tensor.cweb"
;
/*7:*/
#line 107 "./ps_tensor.cweb"

void UPSTensor::addTo(FGSTensor&out)const
{
TL_RAISE_IF(out.getDims()!=tdims,
"Tensors have incompatible dimens in UPSTensor::addTo");
for(index in= out.begin();in!=out.end();++in){
IntSequence vtmp(dimen());
tdims.getPer().apply(in.getCoor(),vtmp);
index tin(this,vtmp);
out.addColumn(*this,*tin,*in);
}
}

/*:7*/
#line 17 "./ps_tensor.cweb"
;
/*8:*/
#line 143 "./ps_tensor.cweb"

void UPSTensor::addTo(UGSTensor&out)const
{
TL_RAISE_IF(out.getDims()!=tdims,
"Tensors have incompatible dimens in UPSTensor::addTo");
int cols= tailIdentitySize();
int off= tdims.tailIdentity();
IntSequence outrun(out.dimen(),0);
IntSequence outrun_part(outrun,0,out.dimen()-off);
IntSequence nvmax_part(out.getDims().getNVX(),0,out.dimen()-off);
for(int out_col= 0;out_col<out.ncols();out_col+= cols){

IntSequence perrun(out.dimen());
tdims.getPer().apply(outrun,perrun);
index from(this,perrun);

ConstTwoDMatrix subfrom(*this,*from,cols);
TwoDMatrix subout(out,out_col,cols);

subout.add(1,subfrom);

UTensor::increment(outrun_part,nvmax_part);
}
}


/*:8*/
#line 18 "./ps_tensor.cweb"
;
/*9:*/
#line 172 "./ps_tensor.cweb"

int UPSTensor::tailIdentitySize()const
{
return tdims.getNVX().mult(dimen()-tdims.tailIdentity(),dimen());
}

/*:9*/
#line 19 "./ps_tensor.cweb"
;
/*10:*/
#line 184 "./ps_tensor.cweb"

void UPSTensor::fillFromSparseOne(const FSSparseTensor&t,const IntSequence&ss,
const IntSequence&coor)
{
IntSequence cumtmp(ss.size());
cumtmp[0]= 0;
for(int i= 1;i<ss.size();i++)
cumtmp[i]= cumtmp[i-1]+ss[i-1];
IntSequence cum(coor.size());
for(int i= 0;i<coor.size();i++)
cum[i]= cumtmp[coor[i]];

zeros();
for(Tensor::index run= begin();run!=end();++run){
IntSequence c(run.getCoor());
c.add(1,cum);
c.sort();
FSSparseTensor::const_iterator sl= t.getMap().lower_bound(c);
if(sl!=t.getMap().end()){
FSSparseTensor::const_iterator su= t.getMap().upper_bound(c);
for(FSSparseTensor::const_iterator srun= sl;srun!=su;++srun)
get((*srun).second.first,*run)= (*srun).second.second;
}
}
}

/*:10*/
#line 20 "./ps_tensor.cweb"
;
/*11:*/
#line 225 "./ps_tensor.cweb"

void UPSTensor::fillFromSparseTwo(const FSSparseTensor&t,const IntSequence&ss,
const IntSequence&coor)
{
IntSequence coor_srt(coor);
coor_srt.sort();
IntSequence cum(ss.size());
cum[0]= 0;
for(int i= 1;i<ss.size();i++)
cum[i]= cum[i-1]+ss[i-1];
IntSequence lb_srt(coor.size());
IntSequence ub_srt(coor.size());
for(int i= 0;i<coor.size();i++){
lb_srt[i]= cum[coor_srt[i]];
ub_srt[i]= cum[coor_srt[i]]+ss[coor_srt[i]]-1;
}

const PermutationSet&pset= tls.pbundle->get(coor.size());
vector<const Permutation*> pp= pset.getPreserving(coor);

Permutation unsort(coor);
zeros();
FSSparseTensor::const_iterator lbi= t.getMap().lower_bound(lb_srt);
FSSparseTensor::const_iterator ubi= t.getMap().upper_bound(ub_srt);
for(FSSparseTensor::const_iterator run= lbi;run!=ubi;++run){
if(lb_srt.lessEq((*run).first)&&(*run).first.lessEq(ub_srt)){
IntSequence c((*run).first);
c.add(-1,lb_srt);
unsort.apply(c);
for(unsigned int i= 0;i<pp.size();i++){
IntSequence cp(coor.size());
pp[i]->apply(c,cp);
Tensor::index ind(this,cp);
TL_RAISE_IF(*ind<0||*ind>=ncols(),
"Internal error in slicing constructor of UPSTensor");
get((*run).second.first,*ind)= (*run).second.second;
}
}
}
}


/*:11*/
#line 21 "./ps_tensor.cweb"
;
/*12:*/
#line 270 "./ps_tensor.cweb"

void PerTensorDimens2::setDimensionSizes()
{
const IntSequence&nvs= getNVS();
for(int i= 0;i<numSyms();i++){
TensorDimens td(syms[i],nvs);
ds[i]= td.calcFoldMaxOffset();
}
}

/*:12*/
#line 22 "./ps_tensor.cweb"
;
/*13:*/
#line 287 "./ps_tensor.cweb"

int PerTensorDimens2::calcOffset(const IntSequence&coor)const
{
TL_RAISE_IF(coor.size()!=dimen(),
"Wrong length of coordinates in PerTensorDimens2::calcOffset");
IntSequence cc(coor);
int ret= 0;
int off= 0;
for(int i= 0;i<numSyms();i++){
TensorDimens td(syms[i],getNVS());
IntSequence c(cc,off,off+syms[i].dimen());
int a= td.calcFoldOffset(c);
ret= ret*ds[i]+a;
off+= syms[i].dimen();
}
return ret;
}

/*:13*/
#line 23 "./ps_tensor.cweb"
;
/*14:*/
#line 306 "./ps_tensor.cweb"

void PerTensorDimens2::print()const
{
printf("nvmax: ");nvmax.print();
printf("per:   ");per.print();
printf("syms:  ");syms.print();
printf("dims:  ");ds.print();
}

/*:14*/
#line 24 "./ps_tensor.cweb"
;
/*15:*/
#line 320 "./ps_tensor.cweb"

void FPSTensor::increment(IntSequence&v)const
{
TL_RAISE_IF(v.size()!=dimen(),
"Wrong length of coordinates in FPSTensor::increment");
UTensor::increment(v,tdims.getNVX());
int off= 0;
for(int i= 0;i<tdims.numSyms();i++){
IntSequence c(v,off,off+tdims.getSym(i).dimen());
c.pmonotone(tdims.getSym(i));
off+= tdims.getSym(i).dimen();
}
}


/*:15*/
#line 25 "./ps_tensor.cweb"
;
/*16:*/
#line 336 "./ps_tensor.cweb"

void FPSTensor::decrement(IntSequence&v)const
{
TL_RAISE("FPSTensor::decrement not implemented");
}

/*:16*/
#line 26 "./ps_tensor.cweb"
;
/*17:*/
#line 343 "./ps_tensor.cweb"

UTensor&FPSTensor::unfold()const
{
TL_RAISE("Unfolding of FPSTensor not implemented");
UFSTensor*nothing= new UFSTensor(0,0,0);
return*nothing;
}

/*:17*/
#line 27 "./ps_tensor.cweb"
;
/*18:*/
#line 352 "./ps_tensor.cweb"

int FPSTensor::getOffset(const IntSequence&v)const
{
return tdims.calcOffset(v);
}

/*:18*/
#line 28 "./ps_tensor.cweb"
;
/*19:*/
#line 364 "./ps_tensor.cweb"

void FPSTensor::addTo(FGSTensor&out)const
{
for(index tar= out.begin();tar!=out.end();++tar){
IntSequence coor(dimen());
tdims.getPer().apply(tar.getCoor(),coor);
index src(this,coor);
out.addColumn(*this,*src,*tar);
}
}

/*:19*/
#line 29 "./ps_tensor.cweb"
;
/*20:*/
#line 395 "./ps_tensor.cweb"

FPSTensor::FPSTensor(const TensorDimens&td,const Equivalence&e,const Permutation&p,
const GSSparseTensor&a,const KronProdAll&kp)
:FTensor(along_col,PerTensorDimens(td,Permutation(e,p)).getNVX(),
a.nrows(),kp.ncols(),td.dimen()),
tdims(td,e,p)
{
zeros();

UGSTensor dummy(0,a.getDims());
for(Tensor::index run= dummy.begin();run!=dummy.end();++run){
Tensor::index fold_ind= dummy.getFirstIndexOf(run);
const IntSequence&c= fold_ind.getCoor();
GSSparseTensor::const_iterator sl= a.getMap().lower_bound(c);
if(sl!=a.getMap().end()){
Vector*row_prod= kp.multRows(run.getCoor());
GSSparseTensor::const_iterator su= a.getMap().upper_bound(c);
for(GSSparseTensor::const_iterator srun= sl;srun!=su;++srun){
Vector out_row((*srun).second.first,*this);
out_row.add((*srun).second.second,*row_prod);
}
delete row_prod;
}
}
}


/*:20*/
#line 30 "./ps_tensor.cweb"
;

/*:1*/
