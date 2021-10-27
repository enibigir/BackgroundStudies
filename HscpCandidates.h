//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Sep 15 15:27:19 2021 by ROOT version 6.14/09
// from TTree HscpCandidates/HscpCandidates
// found on file: /opt/sbg/cms/ui3_data1/dapparu/HSCP/Production/resultSingleMu_TkOnly_UL2017C_v2-4_tree/histoSingleMu_TkOnly_UL2017C_v2-4_tree.root
//////////////////////////////////////////////////////////

#ifndef HscpCandidates_h
#define HscpCandidates_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include "TRandom3.h"

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"
#include "vector"




float K(2.37), C(2.93);


double GetMassErr (double P, double PErr, double dEdx, double dEdxErr, double M, double dEdxK, double dEdxC)
{
   if (M < 0) return -1;
   double KErr     = 0.2;
   //double CErr     = 0.4; UNUSED
   //double cErr     = 0.01; UNUSED
   double Criteria = dEdx - dEdxC;
   double Fac1     = P*P/(2*M*dEdxK);
   double Fac2     = pow(2*M*M*dEdxK/(P*P), 2);
   double MassErr  = Fac1*sqrt(Fac2*pow(PErr/P, 2) + Criteria*Criteria*pow(KErr/dEdxK,2) + dEdxErr*dEdxErr + dEdxC*dEdxC);

   if (std::isnan(MassErr) || std::isinf(MassErr)) MassErr = -1;

   return MassErr/M;
}
  
TH1F* ratioHist(TH1F* h1, TH1F* h2)
{
    TH1F* res = (TH1F*) h1->Clone();
    for(int i=0;i<h1->GetNbinsX();i++)
    {
        double ratio = h2->GetBinContent(i)>0 ? h1->GetBinContent(i)/h2->GetBinContent(i) : 0;
        res->SetBinContent(i,ratio);
    }
    res->Divide(h2);
    res->Sumw2();
    return res;
}

float GetMass(float& p, float& ih, float& k, float& c)
{
    return (ih-C)<0?-1:sqrt((ih-c)/k)*p;
}

void scale(TH1F* h)
{
    h->Scale(1./h->Integral(0,h->GetNbinsX()+1));
}

void invScale(TH1F* h)
{
    //h->Scale(h->Integral(0,h->GetNbinsX()+1));
    h->Sumw2();
    h->Scale(h->GetEntries());
}

TH1F* ratioIntegral(TH1F* h1, TH1F* h2)
{
    TH1F* res = (TH1F*) h1->Clone(); res->Reset();
    for(int i=0;i<h1->GetNbinsX()+1;i++)
    {   
        double Perr=0, Derr=0;
        double P=h1->IntegralAndError(i,h1->GetNbinsX()+1,Perr); if(P<=0) continue;
        double D=h2->IntegralAndError(i,h2->GetNbinsX()+1,Derr);
        res->SetBinContent(i,D/P);
        res->SetBinError(i,sqrt(pow(Derr*P,2)+pow(Perr*D,2))/pow(P,2));
    }
    return res;
}

float chi2test(TH1F* h1, TH1F* h2,int& dof)
{
    float res=0;
    int ndof=0;
    for(int i=1;i<h1->GetNbinsX();i++)
    {
        res += h2->GetBinContent(i)>0 ? pow((h1->GetBinContent(i)-h2->GetBinContent(i)),2)/h2->GetBinContent(i) : 0 ;
        if(h2->GetBinContent(i)>0) ndof++;
    }
    dof=ndof;
    return res/ndof;
}

float invExpoInterpolate(TH1F* h1, float x, std::vector<float> vect)
{
    Int_t xbin = h1->FindBin(x);
    Double_t x0,x1,y0,y1;
           
    if(x<=h1->GetBinCenter(1)) {
        return h1->GetBinContent(1);
    } else if(x>=h1->GetBinCenter(h1->GetNbinsX())) {
        return h1->GetBinContent(h1->GetNbinsX());
    } else {
        /*if(x<=h1->GetBinCenter(xbin)) {
            y0 = h1->RetrieveBinContent(xbin-1);
            x0 = h1->GetBinCenter(xbin-1);
            y1 = h1->RetrieveBinContent(xbin);
            x1 = h1->GetBinCenter(xbin);
        } else {
            y0 = h1->RetrieveBinContent(xbin);
            x0 = h1->GetBinCenter(xbin);
            y1 = h1->RetrieveBinContent(xbin+1);
            x1 = h1->GetBinCenter(xbin+1);
        }*/
        y0 = h1->GetBinContent(xbin-2);
        x0 = vect[xbin-2];
        y1 = h1->GetBinContent(xbin-1);
        x1 = vect[xbin-1];
        
        return y0 + (x-x0)*((y1-y0)/(x1-x0)) > 0 ? y0 + (x-x0)*((y1-y0)/(x1-x0)) : 0;
    }
}

TH1F* histoInterpolated(TH1F* h1)
{
    TH1F* h2 = (TH1F*) h1->Clone(); h2->Reset();
    for(int i=0;i<h1->GetNbinsX()+1;i++)
    {
        h2->SetBinContent(i,h1->Interpolate(h1->GetBinCenter(i)));
    }
    return h2;
}
TCanvas* plotting(TH1F* h1, TH1F* h2, bool ratioSimple=true, std::string name="", std::string leg1="", std::string leg2="")
{
    h1->Sumw2(); h2->Sumw2();
    TCanvas* c1 = new TCanvas(("plotting_"+name).c_str(),"");
    c1->Divide(1,2);
    gStyle->SetOptStat(0);
    c1->cd(1);
    //h1->Rebin(2); h2->Rebin(2);
    TLegend* leg = new TLegend(0.7,0.7,0.9,0.9);
    leg->AddEntry(h1,leg1.c_str(),"lep");
    leg->AddEntry(h2,leg2.c_str(),"lep");
    h1->Draw();
    h2->SetLineColor(2);
    h2->Draw("esame");
    leg->Draw("same");
    c1->SetLogy();
    c1->cd(2);
    TH1F* tmp = (TH1F*) h1->Clone(); tmp->Reset();
    if(ratioSimple)
    {
        h1->Scale(1./h1->Integral());
        h2->Scale(1./h2->Integral());
        //ratioHist(tmp,h1,h2);
        tmp = (TH1F*)h1->Clone();
        tmp->Divide(h2);
        tmp->GetYaxis()->SetTitle("#frac{N_{obs}}{N_{pred}}");
    }
    else
    {
        h1->Scale(1./h1->Integral());
        h2->Scale(1./h2->Integral());
        tmp=ratioIntegral(h2,h1);
        //tmp->GetYaxis()->SetTitle("#frac{#int^{#infty}_{M}{dm_{obs}}}{#int^{#infty}_{M}{dm_{pred}}}");
    }

    int dof;
    float chi2=chi2test(h1,h2,dof);
    /*std::cout << name << std::endl; 
    std::cout << "chi2/ndof: " << chi2/dof << " ndof: " << dof << " chi2: " << chi2 <<std::endl;
    std::cout << "prob chi2: " << TMath::Prob(chi2,dof) << std::endl;
    std::cout << "Kolmogorov: " << h2->KolmogorovTest(h1) << std::endl;*/
    tmp->GetYaxis()->SetRangeUser(0,2);
    tmp->Draw();
    return c1;
}

TH1F* rebinning(TH1F* h1, float min, std::vector<double>& vect)
{
    std::vector<double> v_val;
    int i=0;
    while(i<h1->GetNbinsX()+1)
    {
        float cont=0;
        while(cont<min && i<h1->GetNbinsX()+1)
        {
            cont += h1->GetBinContent(i);
            i++;
        }
        v_val.push_back(h1->GetBinLowEdge(i));
    }

    TH1F* h2 = new TH1F("Rebinned","",v_val.size()-1,v_val.data());
    for(int j=0;j<h1->GetNbinsX()+1;j++)
    {
        for(int c=0;c<h1->GetBinContent(j);c++)
        {
            h2->Fill(h1->GetBinLowEdge(j));
        }
    }
    h2->Sumw2();
    vect = v_val;
    return h2;
}

class region{

    public:
        int nbins;
        float* xbins;
        int np;
        double* xp;
        float plow;
        float pup;
        int npt;
        float ptlow;
        float ptup;
        int nih;
        float ihlow;
        float ihup;
        int nias;
        float iaslow;
        float iasup;
        int neta;
        float etalow;
        float etaup;
        int nmass;
        float masslow;
        float massup;
        std::vector<float> vect;
        region();
        region(std::string suffix,int nbins, float* xbins, std::vector<double> v_pbins, std::vector<float> vect);
        ~region();
        void rebinEtaP(std::vector<double> vect);
        void fill(float& eta, float& nhits, float&p, float& pt, float& ih, float& ias, float& m);
        void fillStdDev();
        void fillQuantile();
        void fillMassFrom1DTemplates();
        void fillMassFrom1DTemplatesEtaBinning();
        void plotMass();
        void write();
        void setBins(int nb,float* b);
        std::string suffix_;
        TCanvas* c;
        TH2F* ih_pt;
        TH2F* ias_pt;
        TH2F* ih_ias;
        TH2F* ih_nhits;
        TH2F* ias_nhits;
        TH2F* eta_pt;
        TH2F* eta_p;
        TH2F* nhits_pt;
        TH2F* eta_nhits;
        TH2F* ih_eta;
        TH2F* ih_p;
        TH2F* ias_p;
        TH1F* stdDevIh_p;
        TH1F* stdDevIas_pt;
        TH1F* quantile99Ih_p;
        TH1F* quantile90Ih_p;
        TH1F* quantile70Ih_p;
        TH1F* quantile50Ih_p;
        TH1F* quantile30Ih_p;
        TH1F* quantile01Ih_p;
        TH1F* quantile99Ias_pt;
        TH1F* quantile90Ias_pt;
        TH1F* quantile70Ias_pt;
        TH1F* quantile50Ias_pt;
        TH1F* quantile30Ias_pt;
        TH1F* quantile01Ias_pt;
        TH1F* mass;
        TH1F* massFrom1DTemplates;
        TH1F* massFrom1DTemplatesEtaBinning;
        TH2F* eta_p_rebinned;
        TH3F* ih_p_eta;
        std::vector<double> VectOfBins_P_;
        TH1F* errMass;

};

region::region()
{
    ih_pt       = 0;
    ias_pt      = 0;
    ih_ias      = 0;
    ih_nhits    = 0;
    ias_nhits   = 0;
    eta_pt      = 0;
    eta_p       = 0;
    nhits_pt    = 0;
    eta_nhits   = 0;
    ih_eta      = 0;
    ih_p        = 0;
    ias_p       = 0;
    stdDevIh_p  = 0;
    stdDevIas_pt= 0;
    mass        = 0;
    massFrom1DTemplates = 0;
    massFrom1DTemplatesEtaBinning = 0;
    eta_p_rebinned = 0;
    quantile99Ih_p      = 0;
    quantile90Ih_p      = 0;
    quantile70Ih_p      = 0;
    quantile50Ih_p      = 0;
    quantile30Ih_p      = 0;
    quantile01Ih_p      = 0;
    quantile99Ias_pt    = 0;
    quantile90Ias_pt    = 0;
    quantile70Ias_pt    = 0;
    quantile50Ias_pt    = 0;
    quantile30Ias_pt    = 0;
    quantile01Ias_pt    = 0;
    ih_p_eta            = 0;
    errMass = new TH1F(("errMass"+suffix_).c_str(),";Mass error",nmass,masslow,massup);
}   

region::region(std::string suffix,int nbins_,float* xbins_,std::vector<double> v_pbins,std::vector<float> vect_)
{
    nbins = nbins_;
    xbins = xbins_;


    np = 2000;
    plow = 0;
    pup = 4000;

    npt = 2000;
    ptlow = 0;
    ptup = 4000;

    nih = 500;
    ihlow = 0;
    ihup = 10;

    nias = 500;
    iaslow = 0;
    iasup = 1;

    neta = 120;
    etalow = -3;
    etaup = 3;

    nmass = 2000;
    masslow = 0;
    massup = 4000;


    suffix_ = suffix;

    vect = vect_;

    c = new TCanvas(suffix.c_str(),"");

    ih_p_eta = new TH3F(("ih_p_eta"+suffix).c_str(),";#eta;p [GeV];I_{h} [MeV/cm]",neta,etalow,etaup,np,plow,pup,nih,ihlow,ihup);

    ih_pt = new TH2F(("ih_pt"+suffix).c_str(),";pt [GeV];I_{h} [MeV/cm]",npt,ptlow,ptup,nih,ihlow,ihup);
    ias_pt = new TH2F(("ias_pt"+suffix).c_str(),";pt [GeV];I_{as}",npt,ptlow,ptup,nias,iaslow,iasup);
    ih_ias = new TH2F(("ias_ih"+suffix).c_str(),";I_{as};I_{h} [MeV/cm]",nias,iaslow,iasup,nih,ihlow,ihup);
    ih_nhits = new TH2F(("ih_nhits"+suffix).c_str(),";nhits;I_{h} [MeV/cm]",20,0,20,nih,ihlow,ihup);
    ias_nhits = new TH2F(("ias_nhits"+suffix).c_str(),";nhits;I_{as}",20,0,20,nias,iaslow,iasup);
    eta_pt = new TH2F(("eta_pt"+suffix).c_str(),";pt [GeV];#eta",npt,ptlow,ptup,neta,etalow,etaup);
    eta_p = new TH2F(("eta_p"+suffix).c_str(),";p [GeV];#eta",np,plow,pup,neta,etalow,etaup);
    eta_p_rebinned = 0;
    //eta_p = new TH2F(("eta_p"+suffix).c_str(),";p [GeV];#eta",v_pbins.size()-1,v_pbins.data(),60,-3,3);
    nhits_pt = new TH2F(("nhits_pt"+suffix).c_str(),";pt [GeV];nhits",npt,ptlow,ptup,20,0,20);
    eta_nhits = new TH2F(("eta_nhits"+suffix).c_str(),";nhits;#eta",20,0,20,neta,etalow,etaup);
    ih_eta = new TH2F(("ih_eta"+suffix).c_str(),";#eta;I_{h} [MeV/cm]",neta,etalow,etaup,nih,ihlow,ihup);
    ih_p = new TH2F(("ih_p"+suffix).c_str(),";p [GeV];I_{h} [MeV/cm]",np,plow,pup,nih,ihlow,ihup);
    //ih_p = new TH2F(("ih_p"+suffix).c_str(),";p [GeV];I_{h} [MeV/cm]",v_pbins.size()-1,v_pbins.data(),100,0,10);
    ias_p = new TH2F(("ias_p"+suffix).c_str(),";p [GeV];I_{as}",np,plow,pup,nias,iaslow,iasup);
    //ias_p = new TH2F(("ias_p"+suffix).c_str(),";p [GeV];I_{as}",v_pbins.size()-1,v_pbins.data(),100,0,1);
    stdDevIh_p = new TH1F(("stdDevIh_p"+suffix).c_str(),";p [GeV];StdDev Ih [MeV/cm]",np,plow,pup);
    //stdDevIh_p = new TH1F(("stdDevIh_p"+suffix).c_str(),";p [GeV];StdDev Ih [MeV/cm]",v_pbins.size()-1,v_pbins.data());
    stdDevIas_pt = new TH1F(("stdDevIas_pt"+suffix).c_str(),";pt [GeV];StdDev I_{as}",npt,ptlow,ptup);
    
    /*quantile99Ih_p = new TH1F(("quantile99Ih_p"+suffix).c_str(),";p [GeV];0.99-quantile Ih [MeV/cm]",np,xp);
    quantile90Ih_p = new TH1F(("quantile90Ih_p"+suffix).c_str(),";p [GeV];0.90-quantile Ih [MeV/cm]",np,xp);
    quantile70Ih_p = new TH1F(("quantile70Ih_p"+suffix).c_str(),";p [GeV];0.70-quantile Ih [MeV/cm]",np,xp);
    quantile50Ih_p = new TH1F(("quantile50Ih_p"+suffix).c_str(),";p [GeV];0.50-quantile Ih [MeV/cm]",np,xp);
    quantile30Ih_p = new TH1F(("quantile30Ih_p"+suffix).c_str(),";p [GeV];0.30-quantile Ih [MeV/cm]",np,xp);
    quantile01Ih_p = new TH1F(("quantile01Ih_p"+suffix).c_str(),";p [GeV];0.01-quantile Ih [MeV/cm]",np,xp);*/

    quantile99Ih_p = new TH1F(("quantile99Ih_p"+suffix).c_str(),";p [GeV];0.99-quantile Ih [MeV/cm]",np,plow,pup);
    quantile90Ih_p = new TH1F(("quantile90Ih_p"+suffix).c_str(),";p [GeV];0.90-quantile Ih [MeV/cm]",np,plow,pup);
    quantile70Ih_p = new TH1F(("quantile70Ih_p"+suffix).c_str(),";p [GeV];0.70-quantile Ih [MeV/cm]",np,plow,pup);
    quantile50Ih_p = new TH1F(("quantile50Ih_p"+suffix).c_str(),";p [GeV];0.50-quantile Ih [MeV/cm]",np,plow,pup);
    quantile30Ih_p = new TH1F(("quantile30Ih_p"+suffix).c_str(),";p [GeV];0.30-quantile Ih [MeV/cm]",np,plow,pup);
    quantile01Ih_p = new TH1F(("quantile01Ih_p"+suffix).c_str(),";p [GeV];0.01-quantile Ih [MeV/cm]",np,plow,pup);

    quantile99Ias_pt = new TH1F(("quantile99Ias_pt"+suffix).c_str(),";pt [GeV];0.99-quantile I_{as}",npt,ptlow,ptup);
    quantile90Ias_pt = new TH1F(("quantile90Ias_pt"+suffix).c_str(),";pt [GeV];0.90-quantile I_{as}",npt,ptlow,ptup);
    quantile70Ias_pt = new TH1F(("quantile70Ias_pt"+suffix).c_str(),";pt [GeV];0.70-quantile I_{as}",npt,ptlow,ptup);
    quantile50Ias_pt = new TH1F(("quantile50Ias_pt"+suffix).c_str(),";pt [GeV];0.50-quantile I_{as}",npt,ptlow,ptup);
    quantile30Ias_pt = new TH1F(("quantile30Ias_pt"+suffix).c_str(),";pt [GeV];0.30-quantile I_{as}",npt,ptlow,ptup);
    quantile01Ias_pt = new TH1F(("quantile01Ias_pt"+suffix).c_str(),";pt [GeV];0.01-quantile I_{as}",npt,ptlow,ptup);
    
    mass = new TH1F(("massFromTree"+suffix).c_str(),";Mass [GeV]",nmass,masslow,massup);
    massFrom1DTemplates = new TH1F(("massFrom1DTemplates"+suffix).c_str(),";Mass [GeV]",nmass,masslow,massup);
    massFrom1DTemplatesEtaBinning = new TH1F(("massFrom1DTemplatesEtaBinning"+suffix).c_str(),";Mass [GeV]",nmass,masslow,massup);
    

    errMass = new TH1F(("errMass"+suffix).c_str(),";Mass error",nmass,masslow,massup);
    //mass = new TH1F(("massFromTree"+suffix).c_str(),";Mass [GeV]",nbins,xbins);
    //massFrom1DTemplates = new TH1F(("massFrom1DTemplates"+suffix).c_str(),";Mass [GeV]",nbins,xbins);
    //massFrom1DTemplatesEtaBinning = new TH1F(("massFrom1DTemplatesEtaBinning"+suffix).c_str(),";Mass [GeV]",nbins,xbins);

}   

region::~region()
{
    delete ih_p_eta;
    delete ih_pt;
    delete ias_pt;
    delete ih_ias;
    delete ih_nhits;
    delete ias_nhits;
    delete eta_pt;
    delete eta_p;
    delete nhits_pt;
    delete eta_nhits;
    delete ih_eta;
    delete ih_p;
    delete ias_p;
    delete stdDevIh_p;
    delete stdDevIas_pt;
    delete quantile01Ih_p;
    delete quantile30Ih_p;
    delete quantile50Ih_p;
    delete quantile70Ih_p;
    delete quantile90Ih_p;
    delete quantile99Ih_p;
    delete quantile01Ias_pt;
    delete quantile30Ias_pt;
    delete quantile50Ias_pt;
    delete quantile70Ias_pt;
    delete quantile90Ias_pt;
    delete quantile99Ias_pt;
    delete mass;
    delete massFrom1DTemplates;
    delete massFrom1DTemplatesEtaBinning;
    delete c;
}

void region::setBins(int nb, float* b)
{
    nbins = nb;
    xbins = b;
}

void region::rebinEtaP(std::vector<double> vect)
{
    //eta_p_rebinned = new TH2F(("eta_p_rebinned"+suffix_).c_str(),";p [GeV];#eta",vect.size()-1,vect.data(),60,-3,3);
    eta_p_rebinned = (TH2F*) eta_p->Clone();
    eta_p_rebinned->Rebin(vect.size()-1,"",vect.data());
    eta_p_rebinned->Sumw2();
}

void region::fill(float& eta, float& nhits, float& p, float& pt, float& ih, float& ias, float& m)
{
   ih_p_eta->Fill(eta,p,ih);
   ih_pt->Fill(pt,ih);
   ias_pt->Fill(pt,ias);
   ih_ias->Fill(ias,ih);
   ih_nhits->Fill(nhits,ih);
   ias_nhits->Fill(nhits,ias);
   eta_pt->Fill(pt,eta);
   eta_p->Fill(p,eta);
   nhits_pt->Fill(pt,nhits);
   eta_nhits->Fill(nhits,eta);
   ih_eta->Fill(eta,ih);
   ih_p->Fill(p,ih);
   ias_p->Fill(p,ias);
   mass->Fill(m);
}

void region::fillStdDev()
{
    for(int i=0;i<ih_p->GetNbinsX();i++)
    {
        float stddev=ih_p->ProjectionY("",i,i+1)->GetStdDev();
        float stddeverr=ih_p->ProjectionY("",i,i+1)->GetStdDevError();
        stdDevIh_p->SetBinContent(i,stddev);
        stdDevIh_p->SetBinError(i,stddeverr);
    }
    stdDevIh_p->Rebin(20);
    for(int i=0;i<ias_pt->GetNbinsX();i++)
    {
        float stddev=ias_pt->ProjectionY("",i,i+1)->GetStdDev();
        float stddeverr=ias_pt->ProjectionY("",i,i+1)->GetStdDevError();
        stdDevIas_pt->SetBinContent(i,stddev);
        stdDevIas_pt->SetBinError(i,stddeverr);
    }
    stdDevIas_pt->Rebin(20);
}

void region::fillQuantile()
{
    int n_quan=6;
    double p[6]={0.01,0.30,0.50,0.70,0.90,0.99};
    double q[6];
    for(int i=0;i<ih_p->GetNbinsX();i++)
    {
        ih_p->ProjectionY("",i,i+1)->GetQuantiles(6,q,p);
        quantile01Ih_p->SetBinContent(i,q[0]);
        quantile30Ih_p->SetBinContent(i,q[1]);
        quantile50Ih_p->SetBinContent(i,q[2]);
        quantile70Ih_p->SetBinContent(i,q[3]);
        quantile90Ih_p->SetBinContent(i,q[4]);
        quantile99Ih_p->SetBinContent(i,q[5]);
    }
    for(int i=0;i<ias_pt->GetNbinsX();i++)
    {
        ias_pt->ProjectionY("",i,i+1)->GetQuantiles(6,q,p);
        quantile01Ias_pt->SetBinContent(i,q[0]);
        quantile30Ias_pt->SetBinContent(i,q[1]);
        quantile50Ias_pt->SetBinContent(i,q[2]);
        quantile70Ias_pt->SetBinContent(i,q[3]);
        quantile90Ias_pt->SetBinContent(i,q[4]);
        quantile99Ias_pt->SetBinContent(i,q[5]);
    }
    
}

void region::fillMassFrom1DTemplates()
{
    TH1F* p = (TH1F*) ih_p->ProjectionX();
    TH1F* ih = (TH1F*) ih_p->ProjectionY();
    scale(p); scale(ih);
    for(int i=1;i<p->GetNbinsX()+1;i++)
    {
        for(int j=1;j<ih->GetNbinsX()+1;j++)
        {
            float mom = p->GetBinCenter(i);
            float dedx = ih->GetBinCenter(j);
            float prob = p->GetBinContent(i) * ih->GetBinContent(j);
            massFrom1DTemplates->Fill(GetMass(mom,dedx,K,C),prob*p->Integral());
        }
    }

    massFrom1DTemplates->Sumw2();
    delete p;
    delete ih;
}

// in order to compute properly the uncertainties we use the methods SetBinContent SetBinError instead of Fill
// as several couples of bins in (p,ih) can provide the same mass estimate we need to properly sum the entries and errors
// for a couple of bins in (p,ih) where the bin content were (N_p,N_ih) the associated quantities should be 
// content: (N_p * N_ih) / N_total, where N_total represents the total number of events in the region (integral of p, ih & mass distributions)
// error: content * sqrt( 1 / N_p + 1 / N_ih ) where we assume Poisson uncertainties in both distributions (independent distributions) and we neglect the uncertainty on N_total
// While combining the input for several couples leading to the same mass: 
// contents are added 
// errors: the sqrt of the squared uncertainties are added
// 

void region::fillMassFrom1DTemplatesEtaBinning() 
{
    errMass = new TH1F(("errMass"+suffix_).c_str(),";Mass error",200,0,100);
    TH1F* eta = (TH1F*) ih_eta->ProjectionX();
    for(int i=1;i<eta->GetNbinsX();i++)
    {
        TH1F* p = (TH1F*) eta_p->ProjectionX("proj_p",i,i);
        if(VectOfBins_P_.size()>1) p = (TH1F*)p->Rebin(VectOfBins_P_.size()-1,"",VectOfBins_P_.data());
        TH1F* ih = (TH1F*) ih_eta->ProjectionY("proj_ih",i,i);
        scale(p); //
        for(int j=1;j<p->GetNbinsX();j++)
        {
            for(int k=1;k<ih->GetNbinsX();k++)
            {
                if(p->GetBinContent(j)<=0) continue;
                if(ih->GetBinContent(k)<=0) continue;
                //p=histoInterpolated(p);
                float mom = p->GetBinCenter(j);
                //float mom = vect[j];
                //std::cout << "vetc: " << vect[j] << " center: " << p->GetBinCenter(j) << std::endl;
                float dedx = ih->GetBinCenter(k);
                //float prob = p->GetBinContent(j) * ih->GetBinContent(k);
                //std::cout << "interpolate: " << p->Interpolate(mom) << " invExpo: " << invExpoInterpolate(p,mom,vect) << std::endl;
                //float probP = mom<=1000?p->Interpolate(mom):invExpoInterpolate(p,mom,vect);
                
                //float probP = p->Interpolate(mom);
                //float probP = p->GetBinContent(j);
                float prob = p->GetBinContent(j) * ih->GetBinContent(k);
                float weight = prob*p->Integral();
                float err_weight = weight*sqrt((1./(ih->GetBinContent(k)))+(1./(p->GetBinContent(j)*ih->Integral())));
                float mass = GetMass(mom,dedx,K,C);
                int bin_mass = massFrom1DTemplatesEtaBinning->FindBin(mass);
                float mass_err = GetMassErr(mom,p->GetBinWidth(j),dedx,ih->GetBinWidth(k),mass,K,C); 
                if(prob>=0)
                {
                    // first version : wrong
                    //massFrom1DTemplatesEtaBinning->Fill(GetMass(mom,dedx,K,C),prob*p->Integral());
                    massFrom1DTemplatesEtaBinning->SetBinContent(bin_mass,massFrom1DTemplatesEtaBinning->GetBinContent(bin_mass)+weight);
                    massFrom1DTemplatesEtaBinning->SetBinError(bin_mass,sqrt(pow(massFrom1DTemplatesEtaBinning->GetBinError(bin_mass),2)+pow(err_weight,2)));
                    errMass->Fill(mass_err);
                }
            }
        }
        delete p;
        delete ih;
    }
    massFrom1DTemplatesEtaBinning->Sumw2();
    //scale(massFrom1DTemplatesEtaBinning);
}

void region::plotMass()
{
    c->Divide(1,2);
    c->cd(1);
    scale(mass); scale(massFrom1DTemplates); scale(massFrom1DTemplatesEtaBinning);
    mass->Draw();
    //massFrom1DTemplates->Draw("same");
    //massFrom1DTemplates->SetLineColor(2);
    massFrom1DTemplatesEtaBinning->Draw("same");
    massFrom1DTemplatesEtaBinning->SetLineColor(2);
    c->SetLogy();
    c->cd(2);
    //TH1F* rat = ratioIntegral(massFrom1DTemplates,mass);
    TH1F* rat2 = ratioIntegral(massFrom1DTemplatesEtaBinning,mass);
    //TH1F* rat3 = ratioHist(massFrom1DTemplatesEtaBinning,mass);
    //rat->Draw();
    rat2->Draw();
    //rat2->Draw("same");
    //rat3->Draw("same");

    //printf(("Kolmogorov test: %f Chi2: %f dof: %f "+suffix_+"\n").c_str(),mass->KolmogorovTest(massFrom1DTemplates),chi2test(mass,massFrom1DTemplates),mass->GetNbinsX());
}

void region::write()
{
    plotMass();
    ih_p_eta->Write();
    ih_pt->Write();
    ias_pt->Write();
    ih_ias->Write();
    ih_nhits->Write();
    ias_nhits->Write();
    eta_pt->Write();
    eta_p->Write();
    nhits_pt->Write();
    eta_nhits->Write();
    ih_eta->Write();
    ih_p->Write();
    ias_p->Write();
    stdDevIh_p->Write();
    stdDevIas_pt->Write();
    quantile01Ih_p->Write();
    quantile30Ih_p->Write();
    quantile50Ih_p->Write();
    quantile70Ih_p->Write();
    quantile90Ih_p->Write();
    quantile99Ih_p->Write();
    quantile01Ias_pt->Write();
    quantile30Ias_pt->Write();
    quantile50Ias_pt->Write();
    quantile70Ias_pt->Write();
    quantile90Ias_pt->Write();
    quantile99Ias_pt->Write();
    mass->Write();
    massFrom1DTemplates->Write();
    massFrom1DTemplatesEtaBinning->Write();

    c->Write();

    ih_pt->ProjectionY()->Write();

    ih_p->ProfileX()->Write();
    ias_p->ProfileX()->Write();
    ih_pt->ProfileX()->Write();
    ias_pt->ProfileX()->Write();

    
}

class HscpCandidates {
public :        
    
    TH1F* hA;
    TH1F* hB;
    TH1F* hC;
    TH1F* hD;

    TH2F* regD_ih;
    TH2F* regD_p; 
    TH2F* regD_mass;

   
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UInt_t          Trig;
   UInt_t          Run;
   ULong64_t       Event;
   UInt_t          Lumi;
   UInt_t          PileUp;
   UInt_t          nofVtx;
   UInt_t          Hscp;
   Float_t         Weight;
   Bool_t          HLT_Mu50;
   Bool_t          HLT_PFMET120_PFMHT120_IDTight;
   Bool_t          HLT_PFHT500_PFMET100_PFMHT100_IDTight;
   Bool_t          HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60;
   Bool_t          HLT_MET105_IsoTrk50;
   Float_t         CaloMET;
   Float_t         RecoPFMET;
   Float_t         RecoPFMHT;
   Float_t         HLTPFMET;
   Float_t         HLTPFMHT;
   vector<bool>    *passCutPt55;
   vector<bool>    *passPreselection_noIsolation_noIh;
   vector<bool>    *passPreselection;
   vector<bool>    *passSelection;
   vector<float>   *Charge;
   vector<float>   *Pt;
   vector<float>   *PtErr;
   vector<float>   *Ias;
   vector<float>   *Ih;
   vector<float>   *Ick;
   vector<float>   *Fmip;
   vector<float>   *ProbQ;
   vector<float>   *TOF;
   vector<float>   *TOFErr;
   vector<unsigned int> *TOF_ndof;
   vector<float>   *DTTOF;
   vector<float>   *DTTOFErr;
   vector<unsigned int> *DTTOF_ndof;
   vector<float>   *CSCTOF;
   vector<float>   *CSCTOFErr;
   vector<unsigned int> *CSCTOF_ndof;
   vector<float>   *Mass;
   vector<float>   *MassErr;
   vector<float>   *dZ;
   vector<float>   *dXY;
   vector<float>   *dR;
   vector<float>   *eta;
   vector<float>   *phi;
   vector<unsigned int> *NOH;
   vector<unsigned int> *NOPH;
   vector<float>   *FOVH;
   vector<unsigned int> *NOMH;
   vector<float>   *FOVHD;
   vector<unsigned int> *NOM;
   vector<float>   *iso_TK;
   vector<float>   *iso_ECAL;
   vector<float>   *iso_HCAL;
   vector<float>   *MuonPFIsolationR03_sumChargedHadronPt;
   vector<float>   *MuonPFIsolationR03_sumNeutralHadronPt;
   vector<float>   *MuonPFIsolationR03_sumPhotonPt;
   vector<float>   *MuonPFIsolationR03_sumPUPt;
   vector<float>   *Ih_noL1;
   vector<float>   *Ih_15drop;
   vector<float>   *Ih_StripOnly;
   vector<float>   *Ih_StripOnly_15drop;
   vector<float>   *Ih_SaturationCorrectionFromFits;
   vector<vector<float> > *clust_charge;
   vector<vector<float> > *clust_pathlength;
   vector<vector<bool> > *clust_ClusterCleaning;
   vector<vector<unsigned int> > *clust_nstrip;
   vector<vector<bool> > *clust_sat254;
   vector<vector<bool> > *clust_sat255;
   vector<vector<unsigned int> > *clust_detid;
   vector<vector<bool> > *clust_isStrip;
   vector<vector<bool> > *clust_isPixel;
   vector<float>   *GenId;
   vector<float>   *GenCharge;
   vector<float>   *GenMass;
   vector<float>   *GenPt;
   vector<float>   *GenEta;
   vector<float>   *GenPhi;

   // List of branches
   TBranch        *b_Trig;   //!
   TBranch        *b_Run;   //!
   TBranch        *b_Event;   //!
   TBranch        *b_Lumi;   //!
   TBranch        *b_PileUp;   //!
   TBranch        *b_nofVtx;   //!
   TBranch        *b_Hscp;   //!
   TBranch        *b_Weight;   //!
   TBranch        *b_HLT_Mu50;   //!
   TBranch        *b_HLT_PFMET120_PFMHT120_IDTight;   //!
   TBranch        *b_HLT_PFHT500_PFMET100_PFMHT100_IDTight;   //!
   TBranch        *b_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60;   //!
   TBranch        *b_HLT_MET105_IsoTrk50;   //!
   TBranch        *b_CaloMET;   //!
   TBranch        *b_RecoPFMET;   //!
   TBranch        *b_RecoPFMHT;   //!
   TBranch        *b_HLTPFMET;   //!
   TBranch        *b_HLTPFMHT;   //!
   TBranch        *b_passCutPt55;   //!
   TBranch        *b_passPreselection_noIsolation_noIh;   //!
   TBranch        *b_passPreselection;   //!
   TBranch        *b_passSelection;   //!
   TBranch        *b_Charge;   //!
   TBranch        *b_Pt;   //!
   TBranch        *b_PtErr;   //!
   TBranch        *b_Ias;   //!
   TBranch        *b_Ih;   //!
   TBranch        *b_Ick;   //!
   TBranch        *b_Fmip;   //!
   TBranch        *b_ProbQ;   //!
   TBranch        *b_TOF;   //!
   TBranch        *b_TOFErr;   //!
   TBranch        *b_TOF_ndof;   //!
   TBranch        *b_DTTOF;   //!
   TBranch        *b_DTTOFErr;   //!
   TBranch        *b_DTTOF_ndof;   //!
   TBranch        *b_CSCTOF;   //!
   TBranch        *b_CSCTOFErr;   //!
   TBranch        *b_CSCTOF_ndof;   //!
   TBranch        *b_Mass;   //!
   TBranch        *b_MassErr;   //!
   TBranch        *b_dZ;   //!
   TBranch        *b_dXY;   //!
   TBranch        *b_dR;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_phi;   //!
   TBranch        *b_NOH;   //!
   TBranch        *b_NOPH;   //!
   TBranch        *b_FOVH;   //!
   TBranch        *b_NOMH;   //!
   TBranch        *b_FOVHD;   //!
   TBranch        *b_NOM;   //!
   TBranch        *b_iso_TK;   //!
   TBranch        *b_iso_ECAL;   //!
   TBranch        *b_iso_HCAL;   //!
   TBranch        *b_MuonPFIsolationR03_sumChargedHadronPt;   //!
   TBranch        *b_MuonPFIsolationR03_sumNeutralHadronPt;   //!
   TBranch        *b_MuonPFIsolationR03_sumPhotonPt;   //!
   TBranch        *b_MuonPFIsolationR03_sumPUPt;   //!
   TBranch        *b_Ih_noL1;   //!
   TBranch        *b_Ih_15drop;   //!
   TBranch        *b_Ih_StripOnly;   //!
   TBranch        *b_Ih_StripOnly_15drop;   //!
   TBranch        *b_Ih_SaturationCorrectionFromFits;   //!
   TBranch        *b_clust_charge;   //!
   TBranch        *b_clust_pathlength;   //!
   TBranch        *b_clust_ClusterCleaning;   //!
   TBranch        *b_clust_nstrip;   //!
   TBranch        *b_clust_sat254;   //!
   TBranch        *b_clust_sat255;   //!
   TBranch        *b_clust_detid;   //!
   TBranch        *b_clust_isStrip;   //!
   TBranch        *b_clust_isPixel;   //!
   TBranch        *b_GenId;   //!
   TBranch        *b_GenCharge;   //!
   TBranch        *b_GenMass;   //!
   TBranch        *b_GenPt;   //!
   TBranch        *b_GenEta;   //!
   TBranch        *b_GenPhi;   //!

   HscpCandidates(TTree *tree=0);
   virtual ~HscpCandidates();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef HscpCandidates_cxx
HscpCandidates::HscpCandidates(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      //TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("histoMET_TkOnly_UL2017C_v2-4_pt50.root");
      //TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("histoSingleMu_TkOnly_UL2017C_v2-4_tree.root");
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("histoSingleMu_TkOnly_UL2017C_v2-5_wTree.root");
      if (!f || !f->IsOpen()) {
         //f = new TFile("histoMET_TkOnly_UL2017C_v2-4_pt50.root");
         //f = new TFile("histoSingleMu_TkOnly_UL2017C_v2-4_tree.root");
         f = new TFile("histoSingleMu_TkOnly_UL2017C_v2-5_wTree.root");
      }
      //TDirectory * dir = (TDirectory*)f->Get("histoMET_TkOnly_UL2017C_v2-4_pt50.root:/analyzer/Data_2017_UL");
      //TDirectory * dir = (TDirectory*)f->Get("histoSingleMu_TkOnly_UL2017C_v2-4_tree.root:/analyzer/Data_2017_UL");
      TDirectory * dir = (TDirectory*)f->Get("histoSingleMu_TkOnly_UL2017C_v2-5_wTree.root:/analyzer/Data_2017_UL");
      dir->GetObject("HscpCandidates",tree);


      regD_ih = (TH2F*) f->Get("histoSingleMu_TkOnly_UL2017C_v2-5_wTree.root:/analyzer/Data_2017_UL/RegionD_I");
      //regD_ih = (TH2F*) f->Get("histoMET_TkOnly_UL2017C_v2-4_pt50.root:/analyzer/Data_2017_UL/RegionD_I");
      regD_p = (TH2F*) f->Get("histoSingleMu_TkOnly_UL2017C_v2-5_wTree.root:/analyzer/Data_2017_UL/RegionD_P");
      //regD_p = (TH2F*) f->Get("histoMET_TkOnly_UL2017C_v2-4_pt50.root:/analyzer/Data_2017_UL/RegionD_P");
      regD_mass = (TH2F*) f->Get("histoSingleMu_TkOnly_UL2017C_v2-5_wTree.root:/analyzer/Data_2017_UL/Mass");
      //regD_mass = (TH2F*) f->Get("histoMET_TkOnly_UL2017C_v2-4_pt50.root:/analyzer/Data_2017_UL/Mass");

      //dir->GetObject("RegionD_I",regD_ih);
      //dir->GetObject("RegionD_P",regD_p);
      //dir->GetObject("Mass",regD_mass);

   }
   Init(tree);
   Loop();
}

HscpCandidates::~HscpCandidates()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t HscpCandidates::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t HscpCandidates::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void HscpCandidates::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   passCutPt55 = 0;
   passPreselection_noIsolation_noIh = 0;
   passPreselection = 0;
   passSelection = 0;
   Charge = 0;
   Pt = 0;
   PtErr = 0;
   Ias = 0;
   Ih = 0;
   Ick = 0;
   Fmip = 0;
   ProbQ = 0;
   TOF = 0;
   TOFErr = 0;
   TOF_ndof = 0;
   DTTOF = 0;
   DTTOFErr = 0;
   DTTOF_ndof = 0;
   CSCTOF = 0;
   CSCTOFErr = 0;
   CSCTOF_ndof = 0;
   Mass = 0;
   MassErr = 0;
   dZ = 0;
   dXY = 0;
   dR = 0;
   eta = 0;
   phi = 0;
   NOH = 0;
   NOPH = 0;
   FOVH = 0;
   NOMH = 0;
   FOVHD = 0;
   NOM = 0;
   iso_TK = 0;
   iso_ECAL = 0;
   iso_HCAL = 0;
   MuonPFIsolationR03_sumChargedHadronPt = 0;
   MuonPFIsolationR03_sumNeutralHadronPt = 0;
   MuonPFIsolationR03_sumPhotonPt = 0;
   MuonPFIsolationR03_sumPUPt = 0;
   Ih_noL1 = 0;
   Ih_15drop = 0;
   Ih_StripOnly = 0;
   Ih_StripOnly_15drop = 0;
   Ih_SaturationCorrectionFromFits = 0;
   clust_charge = 0;
   clust_pathlength = 0;
   clust_ClusterCleaning = 0;
   clust_nstrip = 0;
   clust_sat254 = 0;
   clust_sat255 = 0;
   clust_detid = 0;
   clust_isStrip = 0;
   clust_isPixel = 0;
   GenId = 0;
   GenCharge = 0;
   GenMass = 0;
   GenPt = 0;
   GenEta = 0;
   GenPhi = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Trig", &Trig, &b_Trig);
   fChain->SetBranchAddress("Run", &Run, &b_Run);
   fChain->SetBranchAddress("Event", &Event, &b_Event);
   fChain->SetBranchAddress("Lumi", &Lumi, &b_Lumi);
   fChain->SetBranchAddress("PileUp", &PileUp, &b_PileUp);
   fChain->SetBranchAddress("nofVtx", &nofVtx, &b_nofVtx);
   fChain->SetBranchAddress("Hscp", &Hscp, &b_Hscp);
/*   fChain->SetBranchAddress("Weight", &Weight, &b_Weight);
   fChain->SetBranchAddress("HLT_Mu50", &HLT_Mu50, &b_HLT_Mu50);
   fChain->SetBranchAddress("HLT_PFMET120_PFMHT120_IDTight", &HLT_PFMET120_PFMHT120_IDTight, &b_HLT_PFMET120_PFMHT120_IDTight);
   fChain->SetBranchAddress("HLT_PFHT500_PFMET100_PFMHT100_IDTight", &HLT_PFHT500_PFMET100_PFMHT100_IDTight, &b_HLT_PFHT500_PFMET100_PFMHT100_IDTight);
   fChain->SetBranchAddress("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60", &HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60, &b_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60);
   fChain->SetBranchAddress("HLT_MET105_IsoTrk50", &HLT_MET105_IsoTrk50, &b_HLT_MET105_IsoTrk50);
   fChain->SetBranchAddress("CaloMET", &CaloMET, &b_CaloMET);
   fChain->SetBranchAddress("RecoPFMET", &RecoPFMET, &b_RecoPFMET);
   fChain->SetBranchAddress("RecoPFMHT", &RecoPFMHT, &b_RecoPFMHT);
   fChain->SetBranchAddress("HLTPFMET", &HLTPFMET, &b_HLTPFMET);
   fChain->SetBranchAddress("HLTPFMHT", &HLTPFMHT, &b_HLTPFMHT);*/
   fChain->SetBranchAddress("passCutPt55", &passCutPt55, &b_passCutPt55);
   fChain->SetBranchAddress("passPreselection_noIsolation_noIh", &passPreselection_noIsolation_noIh, &b_passPreselection_noIsolation_noIh);
   fChain->SetBranchAddress("passPreselection", &passPreselection, &b_passPreselection);
   fChain->SetBranchAddress("passSelection", &passSelection, &b_passSelection);
   fChain->SetBranchAddress("Charge", &Charge, &b_Charge);
   fChain->SetBranchAddress("Pt", &Pt, &b_Pt);
   fChain->SetBranchAddress("PtErr", &PtErr, &b_PtErr);
   fChain->SetBranchAddress("Ias", &Ias, &b_Ias);
   fChain->SetBranchAddress("Ih", &Ih, &b_Ih);
   fChain->SetBranchAddress("Ick", &Ick, &b_Ick);
   fChain->SetBranchAddress("Fmip", &Fmip, &b_Fmip);
   fChain->SetBranchAddress("ProbQ", &ProbQ, &b_ProbQ);
   fChain->SetBranchAddress("TOF", &TOF, &b_TOF);
/*   fChain->SetBranchAddress("TOFErr", &TOFErr, &b_TOFErr);
   fChain->SetBranchAddress("TOF_ndof", &TOF_ndof, &b_TOF_ndof);
   fChain->SetBranchAddress("DTTOF", &DTTOF, &b_DTTOF);
   fChain->SetBranchAddress("DTTOFErr", &DTTOFErr, &b_DTTOFErr);
   fChain->SetBranchAddress("DTTOF_ndof", &DTTOF_ndof, &b_DTTOF_ndof);
   fChain->SetBranchAddress("CSCTOF", &CSCTOF, &b_CSCTOF);
   fChain->SetBranchAddress("CSCTOFErr", &CSCTOFErr, &b_CSCTOFErr);
   fChain->SetBranchAddress("CSCTOF_ndof", &CSCTOF_ndof, &b_CSCTOF_ndof);*/
   fChain->SetBranchAddress("Mass", &Mass, &b_Mass);
   fChain->SetBranchAddress("MassErr", &MassErr, &b_MassErr);
   fChain->SetBranchAddress("dZ", &dZ, &b_dZ);
   fChain->SetBranchAddress("dXY", &dXY, &b_dXY);
   fChain->SetBranchAddress("dR", &dR, &b_dR);
   fChain->SetBranchAddress("eta", &eta, &b_eta);
   fChain->SetBranchAddress("phi", &phi, &b_phi);
   fChain->SetBranchAddress("NOH", &NOH, &b_NOH);
   fChain->SetBranchAddress("NOPH", &NOPH, &b_NOPH);
   fChain->SetBranchAddress("FOVH", &FOVH, &b_FOVH);
   fChain->SetBranchAddress("NOMH", &NOMH, &b_NOMH);
   fChain->SetBranchAddress("FOVHD", &FOVHD, &b_FOVHD);
   fChain->SetBranchAddress("NOM", &NOM, &b_NOM);
   fChain->SetBranchAddress("iso_TK", &iso_TK, &b_iso_TK);
   fChain->SetBranchAddress("iso_ECAL", &iso_ECAL, &b_iso_ECAL);
   fChain->SetBranchAddress("iso_HCAL", &iso_HCAL, &b_iso_HCAL);
   fChain->SetBranchAddress("MuonPFIsolationR03_sumChargedHadronPt", &MuonPFIsolationR03_sumChargedHadronPt, &b_MuonPFIsolationR03_sumChargedHadronPt);
   fChain->SetBranchAddress("MuonPFIsolationR03_sumNeutralHadronPt", &MuonPFIsolationR03_sumNeutralHadronPt, &b_MuonPFIsolationR03_sumNeutralHadronPt);
   fChain->SetBranchAddress("MuonPFIsolationR03_sumPhotonPt", &MuonPFIsolationR03_sumPhotonPt, &b_MuonPFIsolationR03_sumPhotonPt);
   fChain->SetBranchAddress("MuonPFIsolationR03_sumPUPt", &MuonPFIsolationR03_sumPUPt, &b_MuonPFIsolationR03_sumPUPt);
/*   fChain->SetBranchAddress("Ih_noL1", &Ih_noL1, &b_Ih_noL1);
   fChain->SetBranchAddress("Ih_15drop", &Ih_15drop, &b_Ih_15drop);
   fChain->SetBranchAddress("Ih_StripOnly", &Ih_StripOnly, &b_Ih_StripOnly);
   fChain->SetBranchAddress("Ih_StripOnly_15drop", &Ih_StripOnly_15drop, &b_Ih_StripOnly_15drop);
   fChain->SetBranchAddress("Ih_SaturationCorrectionFromFits", &Ih_SaturationCorrectionFromFits, &b_Ih_SaturationCorrectionFromFits);
   fChain->SetBranchAddress("clust_charge", &clust_charge, &b_clust_charge);
   fChain->SetBranchAddress("clust_pathlength", &clust_pathlength, &b_clust_pathlength);
   fChain->SetBranchAddress("clust_ClusterCleaning", &clust_ClusterCleaning, &b_clust_ClusterCleaning);
   fChain->SetBranchAddress("clust_nstrip", &clust_nstrip, &b_clust_nstrip);
   fChain->SetBranchAddress("clust_sat254", &clust_sat254, &b_clust_sat254);
   fChain->SetBranchAddress("clust_sat255", &clust_sat255, &b_clust_sat255);
   fChain->SetBranchAddress("clust_detid", &clust_detid, &b_clust_detid);
   fChain->SetBranchAddress("clust_isStrip", &clust_isStrip, &b_clust_isStrip);
   fChain->SetBranchAddress("clust_isPixel", &clust_isPixel, &b_clust_isPixel);
   fChain->SetBranchAddress("GenId", &GenId, &b_GenId);
   fChain->SetBranchAddress("GenCharge", &GenCharge, &b_GenCharge);
   fChain->SetBranchAddress("GenMass", &GenMass, &b_GenMass);
   fChain->SetBranchAddress("GenPt", &GenPt, &b_GenPt);
   fChain->SetBranchAddress("GenEta", &GenEta, &b_GenEta);
   fChain->SetBranchAddress("GenPhi", &GenPhi, &b_GenPhi);*/
   Notify();
}

Bool_t HscpCandidates::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void HscpCandidates::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t HscpCandidates::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef HscpCandidates_cxx