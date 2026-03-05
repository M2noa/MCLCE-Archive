#include "ShaderConstants.fxh"

#if defined(GAUSSIAN_H) || defined(GAUSSIAN_V)

//Optimized bilinear Gaussian Filters. The idea is to use to hardware bilinear filter to get "free" lerp between pixels. 
//Here we calculate the pixel offset such that we get the desire lerp amount between two pixels.
//std=1.0
#define GAUSSIAN_5_TAB_SUM 1.0
#define G5TABS0 0.0625
#define G5TABS1 0.25
#define G5TABS_CENTER 0.375
#define G5TABS_COUNT 1
static const float G5Offsets[G5TABS_COUNT] = { (2.0*G5TABS0 + G5TABS1) / (G5TABS0 + G5TABS1) };
static const float G5Weights[G5TABS_COUNT] = { G5TABS0 + G5TABS1 };


//std=2.0
#define GAUSSIAN_11_TAB_SUM 1.0
#define G11TABS0 0.008580797401
#define G11TABS1 0.02788759155
#define G11TABS2 0.06693021973
#define G11TABS3 0.1227054028
#define G11TABS4 0.1752934326
#define G11TABS_CENTER 0.1972051117
#define G11TABS_COUNT 3
static const float G11Offsets[G11TABS_COUNT] = { (5.0*G11TABS0 + 4.0*G11TABS1) / (G11TABS0 + G11TABS1), (3.0*G11TABS2 + 2.0*G11TABS3) / (G11TABS2 + G11TABS3), (G11TABS4) / (G11TABS4 + G11TABS_CENTER) };
static const float G11Weights[G11TABS_COUNT] = { G11TABS0 + G11TABS1, G11TABS2 + G11TABS3, G11TABS4 + G11TABS_CENTER };


//std=3.0
#define GAUSSIAN_15_TAB_SUM 1.0
#define G15TABS0 0.008843068404
#define G15TABS1 0.01842305918
#define G15TABS2 0.03401180156
#define G15TABS3 0.05587653113
#define G15TABS4 0.08195224565
#define G15TABS5 0.1075623224
#define G15TABS6 0.1265439087
#define G15TABS_CENTER 0.1335741259
#define G15TABS_COUNT 4
static const float G15Offsets[G15TABS_COUNT] = { (7.0*G15TABS0 + 6.0*G15TABS1) / (G15TABS0 + G15TABS1), (5.0*G15TABS2 + 4.0*G15TABS3) / (G15TABS2 + G15TABS3), (3.0*G15TABS4 + 2.0*G15TABS5) / (G15TABS4 + G15TABS5), (G15TABS6) / (G15TABS6 + G15TABS_CENTER) };
static const float G15Weights[G15TABS_COUNT] = { G15TABS0 + G15TABS1, G15TABS2 + G15TABS3, G15TABS4 + G15TABS5, G15TABS6 + G15TABS_CENTER };


//std=4.0
#define GAUSSIAN_21_TAB_SUM 1.0
#define G21TABS0 0.00439142038
#define G21TABS1 0.008019115477
#define G21TABS2 0.01369932227
#define G21TABS3 0.02191891564
#define G21TABS4 0.03287837346
#define G21TABS5 0.04627326635
#define G21TABS6 0.06114681624
#define G21TABS7 0.07590639258
#define G21TABS8 0.08855745801
#define G21TABS9 0.09712753459
#define G21TABS_CENTER 0.10016277
#define G21TABS_COUNT 5
static const float G21Offsets[G21TABS_COUNT] = { (10.0*G21TABS0 + 9.0*G21TABS1) / (G21TABS0 + G21TABS1), (8.0*G21TABS2 + 7.0*G21TABS3) / (G21TABS2 + G21TABS3), (6.0*G21TABS4 + 5.0*G21TABS5) / (G21TABS4 + G21TABS5), (4.0*G21TABS6 + 3.0*G21TABS7) / (G21TABS6 + G21TABS7), (2.0*G21TABS8 + G21TABS9) / (G21TABS8 + G21TABS9) };
static const float G21Weights[G21TABS_COUNT] = { G21TABS0 + G21TABS1, G21TABS2 + G21TABS3, G21TABS4 + G21TABS5, G21TABS6 + G21TABS7, G21TABS8 + G21TABS9 };


//std=5.0
#define GAUSSIAN_25_TAB_SUM 1.0
#define G25TABS0 0.00452735756
#define G25TABS1 0.007197337659
#define G25TABS2 0.01097593993
#define G25TABS3 0.01606235112
#define G25TABS4 0.02256377895
#define G25TABS5 0.03043486463
#define G25TABS6 0.03942698373
#define G25TABS7 0.04906469086
#define G25TABS8 0.05866430429
#define G25TABS9 0.0674015411
#define G25TABS10 0.07442253496
#define G25TABS11 0.0789790167
#define G25TABS_CENTER 0.08055859703
#define G25TABS_COUNT 6
static const float G25Offsets[G25TABS_COUNT] = { (12.0*G25TABS0 + 11.0*G25TABS1) / (G25TABS0 + G25TABS1), (10.0*G25TABS2 + 9.0*G25TABS3) / (G25TABS2 + G25TABS3), (8.0*G25TABS4 + 7.0*G25TABS5) / (G25TABS4 + G25TABS5), (6.0*G25TABS6 + 5.0*G25TABS7) / (G25TABS6 + G25TABS7), (4.0*G25TABS8 + 3.0*G25TABS9) / (G25TABS8 + G25TABS9), (2.0*G25TABS10 + G25TABS11) / (G25TABS10 + G25TABS11) };
static const float G25Weights[G25TABS_COUNT] = { G25TABS0 + G25TABS1, G25TABS2 + G25TABS3, G25TABS4 + G25TABS5, G25TABS6 + G25TABS7, G25TABS8 + G25TABS9, G25TABS10 + G25TABS11 };


//std=6.0
#define GAUSSIAN_29_TAB_SUM 1.0
#define G29TABS0 0.004438649238
#define G29TABS1 0.006469895499
#define G29TABS2 0.00916568529
#define G29TABS3 0.01262159942
#define G29TABS4 0.01689665728
#define G29TABS5 0.02199247456
#define G29TABS6 0.02783422561
#define G29TABS7 0.03425750845
#define G29TABS8 0.0410051995
#define G29TABS9 0.04773739644
#define G29TABS10 0.05405558126
#define G29TABS11 0.05953948081
#define G29TABS12 0.06379230087
#define G29TABS13 0.0664877502
#define G29TABS_CENTER 0.06741119117
#define G29TABS_COUNT 7
static const float G29Offsets[G29TABS_COUNT] = { (14.0*G29TABS0 + 13.0*G29TABS1) / (G29TABS0 + G29TABS1), (12.0*G29TABS2 + 11.0*G29TABS3) / (G29TABS2 + G29TABS3), (10.0*G29TABS4 + 9.0*G29TABS5) / (G29TABS4 + G29TABS5), (8.0*G29TABS6 + 7.0*G29TABS7) / (G29TABS6 + G29TABS7), (6.0*G29TABS8 + 5.0*G29TABS9) / (G29TABS8 + G29TABS9), (4.0*G29TABS10 + 3.0*G29TABS11) / (G29TABS10 + G29TABS11), (2.0*G29TABS12 + G29TABS13) / (G29TABS12 + G29TABS13) };
static const float G29Weights[G29TABS_COUNT] = { G29TABS0 + G29TABS1, G29TABS2 + G29TABS3, G29TABS4 + G29TABS5, G29TABS6 + G29TABS7, G29TABS8 + G29TABS9, G29TABS10 + G29TABS11, G29TABS12 + G29TABS13 };


//std=7.0
#define GAUSSIAN_33_TAB_SUM 1.0
#define G33TABS0 0.004260871965
#define G33TABS1 0.005852281976
#define G33TABS2 0.007872712659
#define G33TABS3 0.01037345668
#define G33TABS4 0.01338899641
#define G33TABS5 0.01692861615
#define G33TABS6 0.02096839955
#define G33TABS7 0.02544479946
#define G33TABS8 0.03025103936
#define G33TABS9 0.03523747441
#define G33TABS10 0.04021668276
#define G33TABS11 0.04497349469
#define G33TABS12 0.04927946759
#define G33TABS13 0.05291058625
#define G33TABS14 0.05566634596
#define G33TABS15 0.05738798552
#define G33TABS_CENTER 0.05797357721
#define G33TABS_COUNT 8
static const float G33Offsets[G33TABS_COUNT] = { (16.0*G33TABS0 + 15.0*G33TABS1) / (G33TABS0 + G33TABS1), (14.0*G33TABS2 + 13.0*G33TABS3) / (G33TABS2 + G33TABS3), (12.0*G33TABS4 + 11.0*G33TABS5) / (G33TABS4 + G33TABS5), (10.0*G33TABS6 + 9.0*G33TABS7) / (G33TABS6 + G33TABS7), (8.0*G33TABS8 + 7.0*G33TABS9) / (G33TABS8 + G33TABS9), (6.0*G33TABS10 + 5.0*G33TABS11) / (G33TABS10 + G33TABS11), (4.0*G33TABS12 + 3.0*G33TABS13) / (G33TABS12 + G33TABS13), (2.0*G33TABS14 + G33TABS15) / (G33TABS14 + G33TABS15) };
static const float G33Weights[G33TABS_COUNT] = { G33TABS0 + G33TABS1, G33TABS2 + G33TABS3, G33TABS4 + G33TABS5, G33TABS6 + G33TABS7, G33TABS8 + G33TABS9, G33TABS10 + G33TABS11, G33TABS12 + G33TABS13, G33TABS14 + G33TABS15 };


//std=8.0
#define GAUSSIAN_37_TAB_SUM 1.0
#define G37TABS0 0.004053106611
#define G37TABS1 0.0053311132
#define G37TABS2 0.006901887625
#define G37TABS3 0.008795325822
#define G37TABS4 0.01103273327
#define G37TABS5 0.01362302717
#define G37TABS6 0.0165590244
#define G37TABS7 0.01981421723
#define G37TABS8 0.02334047623
#define G37TABS9 0.02706710689
#define G37TABS10 0.0309016137
#define G37TABS11 0.03473239226
#define G37TABS12 0.03843338487
#define G37TABS13 0.04187051685
#define G37TABS14 0.04490950598
#define G37TABS15 0.04742443831
#define G37TABS16 0.04930636047
#define G37TABS17 0.05047107765
#define G37TABS_CENTER 0.05086538294
#define G37TABS_COUNT 9
static const float G37Offsets[G37TABS_COUNT] = { (18.0*G37TABS0 + 17.0*G37TABS1) / (G37TABS0 + G37TABS1), (16.0*G37TABS2 + 15.0*G37TABS3) / (G37TABS2 + G37TABS3), (14.0*G37TABS4 + 13.0*G37TABS5) / (G37TABS4 + G37TABS5), (12.0*G37TABS6 + 11.0*G37TABS7) / (G37TABS6 + G37TABS7), (10.0*G37TABS8 + 9.0*G37TABS9) / (G37TABS8 + G37TABS9), (8.0*G37TABS10 + 7.0*G37TABS11) / (G37TABS10 + G37TABS11), (6.0*G37TABS12 + 5.0*G37TABS13) / (G37TABS12 + G37TABS13), (4.0*G37TABS14 + 3.0*G37TABS15) / (G37TABS14 + G37TABS15), (2.0*G37TABS16 + G37TABS17) / (G37TABS16 + G37TABS17) };
static const float G37Weights[G37TABS_COUNT] = { G37TABS0 + G37TABS1, G37TABS2 + G37TABS3, G37TABS4 + G37TABS5, G37TABS6 + G37TABS7, G37TABS8 + G37TABS9, G37TABS10 + G37TABS11, G37TABS12 + G37TABS13, G37TABS14 + G37TABS15, G37TABS16 + G37TABS17 };


//std=9.0
#define GAUSSIAN_39_TAB_SUM 1.0
#define G39TABS0 0.00492716991
#define G39TABS1 0.006193178845
#define G39TABS2 0.007688084084
#define G39TABS3 0.009425801719
#define G39TABS4 0.01141355582
#define G39TABS5 0.01364999582
#define G39TABS6 0.01612348499
#define G39TABS7 0.01881073249
#define G39TABS8 0.0216759434
#define G39TABS9 0.0246706461
#define G39TABS10 0.02773432111
#define G39TABS11 0.03079590201
#define G39TABS12 0.03377615059
#define G39TABS13 0.03659082981
#define G39TABS14 0.03915451852
#define G39TABS15 0.0413848392
#define G39TABS16 0.04320681325
#define G39TABS17 0.04455702616
#define G39TABS18 0.04538728131
#define G39TABS_CENTER 0.04566744971
#define G39TABS_COUNT 10
static const float G39Offsets[G39TABS_COUNT] = { (19.0*G39TABS0 + 18.0*G39TABS1) / (G39TABS0 + G39TABS1), (17.0*G39TABS2 + 16.0*G39TABS3) / (G39TABS2 + G39TABS3), (15.0*G39TABS4 + 14.0*G39TABS5) / (G39TABS4 + G39TABS5), (13.0*G39TABS6 + 12.0*G39TABS7) / (G39TABS6 + G39TABS7), (11.0*G39TABS8 + 10.0*G39TABS9) / (G39TABS8 + G39TABS9), (9.0*G39TABS10 + 8.0*G39TABS11) / (G39TABS10 + G39TABS11), (7.0*G39TABS12 + 6.0*G39TABS13) / (G39TABS12 + G39TABS13), (5.0*G39TABS14 + 4.0*G39TABS15) / (G39TABS14 + G39TABS15), (3.0*G39TABS16 + 2.0*G39TABS17) / (G39TABS16 + G39TABS17), (G39TABS18) / (G39TABS18 + G39TABS_CENTER) };
static const float G39Weights[G39TABS_COUNT] = { G39TABS0 + G39TABS1, G39TABS2 + G39TABS3, G39TABS4 + G39TABS5, G39TABS6 + G39TABS7, G39TABS8 + G39TABS9, G39TABS10 + G39TABS11, G39TABS12 + G39TABS13, G39TABS14 + G39TABS15, G39TABS16 + G39TABS17, G39TABS18 + G39TABS_CENTER };


//std=10.0
#define GAUSSIAN_43_TAB_SUM 1.0
#define G43TABS0 0.004544882447
#define G43TABS1 0.005580105671
#define G43TABS2 0.006782448882
#define G43TABS3 0.00816129838
#define G43TABS4 0.009722202442
#define G43TABS5 0.01146585831
#define G43TABS6 0.0133871643
#define G43TABS7 0.01547441035
#define G43TABS8 0.0177086835
#define G43TABS9 0.02006356163
#define G43TABS10 0.02250515907
#define G43TABS11 0.02499257139
#define G43TABS12 0.02747874342
#define G43TABS13 0.02991175716
#define G43TABS14 0.03223650512
#define G43TABS15 0.0343966833
#define G43TABS16 0.03633700902
#define G43TABS17 0.03800554515
#define G43TABS18 0.039355996
#define G43TABS19 0.04034983428
#define G43TABS20 0.04095812324
#define G43TABS_CENTER 0.04116291385
#define G43TABS_COUNT 11
static const float G43Offsets[G43TABS_COUNT] = { (21.0*G43TABS0 + 20.0*G43TABS1) / (G43TABS0 + G43TABS1), (19.0*G43TABS2 + 18.0*G43TABS3) / (G43TABS2 + G43TABS3), (17.0*G43TABS4 + 16.0*G43TABS5) / (G43TABS4 + G43TABS5), (15.0*G43TABS6 + 14.0*G43TABS7) / (G43TABS6 + G43TABS7), (13.0*G43TABS8 + 12.0*G43TABS9) / (G43TABS8 + G43TABS9), (11.0*G43TABS10 + 10.0*G43TABS11) / (G43TABS10 + G43TABS11), (9.0*G43TABS12 + 8.0*G43TABS13) / (G43TABS12 + G43TABS13), (7.0*G43TABS14 + 6.0*G43TABS15) / (G43TABS14 + G43TABS15), (5.0*G43TABS16 + 4.0*G43TABS17) / (G43TABS16 + G43TABS17), (3.0*G43TABS18 + 2.0*G43TABS19) / (G43TABS18 + G43TABS19), (G43TABS20) / (G43TABS20 + G43TABS_CENTER) };
static const float G43Weights[G43TABS_COUNT] = { G43TABS0 + G43TABS1, G43TABS2 + G43TABS3, G43TABS4 + G43TABS5, G43TABS6 + G43TABS7, G43TABS8 + G43TABS9, G43TABS10 + G43TABS11, G43TABS12 + G43TABS13, G43TABS14 + G43TABS15, G43TABS16 + G43TABS17, G43TABS18 + G43TABS19, G43TABS20 + G43TABS_CENTER };

#endif


struct PS_Input {
	float4 position : SV_Position;
	float2 uv : TEXCOORD_0_FB_MSAA;
};

struct PS_Output {
	float4 color : SV_Target;
};


void main(in PS_Input PSInput, out PS_Output PSOutput) {

#if defined(GAUSSIAN_H) || defined(GAUSSIAN_V)

#ifdef GAUSSIAN_H
	float2 texOffset = float2(1.0f, 0.0f) / TEXTURE_DIMENSIONS.xy;
#else //GAUSSIAN_V
	float2 texOffset = float2(0.0f, 1.0f) / TEXTURE_DIMENSIONS.xy;
#endif

	if (GaussianBlurSize == 1) {	//Total 3 texture samples per fragment
									//Start with the center Guassian number
		float4 color = G5TABS_CENTER * TEXTURE_0.Sample(TextureSampler0, PSInput.uv);
		//Add the other ones 
		[unroll(G5TABS_COUNT)]
		for (int i = 0; i < G5TABS_COUNT; ++i) {
			color += G5Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv - texOffset * G5Offsets[i]);
			color += G5Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv + texOffset * G5Offsets[i]);
		}
		//No need to divide by total weight as gaussianFilter adds up to 1.0 
		PSOutput.color = color;
	}
	else if (GaussianBlurSize == 2) {	//Total 7 texture samples per fragment
										//Start with the center Guassian number. This is negative because we are including the center as 
										//the bilinear calculation below. It will be double counted if we don't first offset it by negative amount
		float4 color = -(G11TABS_CENTER * TEXTURE_0.Sample(TextureSampler0, PSInput.uv));
		//Add the other ones 
		[unroll(G11TABS_COUNT)]
		for (int i = 0; i < G11TABS_COUNT; ++i) {
			color += G11Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv - texOffset * G11Offsets[i]);
			color += G11Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv + texOffset * G11Offsets[i]);
		}
		//No need to divide by total weight as gaussianFilter adds up to 1.0 
		PSOutput.color = color;
	}
	else if (GaussianBlurSize == 3) {	//Total 9 texture samples per fragment
										//Start with the center Guassian number. This is negative because we are including the center as 
										//the bilinear calculation below. It will be double counted if we don't first offset it by negative amount
		float4 color = -(G15TABS_CENTER * TEXTURE_0.Sample(TextureSampler0, PSInput.uv));
		//Add the other ones 
		[unroll(G15TABS_COUNT)]
		for (int i = 0; i < G15TABS_COUNT; ++i) {
			color += G15Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv - texOffset * G15Offsets[i]);
			color += G15Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv + texOffset * G15Offsets[i]);
		}
		//No need to divide by total weight as gaussianFilter adds up to 1.0 
		PSOutput.color = color;
	}
	else if (GaussianBlurSize == 4) {	//Total 11 texture samples per fragment
										//Start with the center Guassian number.
		float4 color = G21TABS_CENTER * TEXTURE_0.Sample(TextureSampler0, PSInput.uv);
		//Add the other ones 
		[unroll(G21TABS_COUNT)]
		for (int i = 0; i < G21TABS_COUNT; ++i) {
			color += G21Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv - texOffset * G21Offsets[i]);
			color += G21Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv + texOffset * G21Offsets[i]);
		}
		//No need to divide by total weight as gaussianFilter adds up to 1.0 
		PSOutput.color = color;
	}
	else if (GaussianBlurSize == 5) {	//Total 13 texture samples per fragment
										//Start with the center Guassian number
		float4 color = G25TABS_CENTER * TEXTURE_0.Sample(TextureSampler0, PSInput.uv);
		//Add the other ones 
		[unroll(G25TABS_COUNT)]
		for (int i = 0; i < G25TABS_COUNT; ++i) {
			color += G25Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv - texOffset * G25Offsets[i]);
			color += G25Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv + texOffset * G25Offsets[i]);
		}
		//No need to divide by total weight as gaussianFilter adds up to 1.0 
		PSOutput.color = color;
	}
	else if (GaussianBlurSize == 6) {	//Total 15 texture samples per fragment
										// Start with the center Guassian number
		float4 color = G29TABS_CENTER * TEXTURE_0.Sample(TextureSampler0, PSInput.uv);
		//Add the other ones 
		[unroll(G29TABS_COUNT)]
		for (int i = 0; i < G29TABS_COUNT; ++i) {
			color += G29Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv - texOffset * G29Offsets[i]);
			color += G29Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv + texOffset * G29Offsets[i]);
		}
		//No need to divide by total weight as gaussianFilter adds up to 1.0 
		PSOutput.color = color;
	}
	else if (GaussianBlurSize == 7) {	//Total 17 texture samples per fragment
										//Start with the center Guassian number. This is negative because we are including the center as 
		float4 color = G33TABS_CENTER * TEXTURE_0.Sample(TextureSampler0, PSInput.uv);
		//Add the other ones 
		[unroll(G33TABS_COUNT)]
		for (int i = 0; i < G33TABS_COUNT; ++i) {
			color += G33Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv - texOffset * G33Offsets[i]);
			color += G33Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv + texOffset * G33Offsets[i]);
		}
		//No need to divide by total weight as gaussianFilter adds up to 1.0 
		PSOutput.color = color;
	}
	else if (GaussianBlurSize == 8) {	//Total 19 texture samples per fragment
										//Start with the center Guassian number. This is negative because we are including the center as 
		float4 color = G37TABS_CENTER * TEXTURE_0.Sample(TextureSampler0, PSInput.uv);
		//Add the other ones 
		[unroll(G37TABS_COUNT)]
		for (int i = 0; i < G37TABS_COUNT; ++i) {
			color += G37Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv - texOffset * G37Offsets[i]);
			color += G37Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv + texOffset * G37Offsets[i]);
		}
		//No need to divide by total weight as gaussianFilter adds up to 1.0 
		PSOutput.color = color;
	}
	else if (GaussianBlurSize == 9) {	//Total 21 texture samples per fragment
										//Start with the center Guassian number. This is negative because we are including the center as 
										//the bilinear calculation below. It will be double counted if we don't first offset it by negative amount
		float4 color = -(G39TABS_CENTER * TEXTURE_0.Sample(TextureSampler0, PSInput.uv));
		//Add the other ones 
		[unroll(G39TABS_COUNT)]
		for (int i = 0; i < G39TABS_COUNT; ++i) {
			color += G39Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv - texOffset * G39Offsets[i]);
			color += G39Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv + texOffset * G39Offsets[i]);
		}
		//No need to divide by total weight as gaussianFilter adds up to 1.0 
		PSOutput.color = color;
	}
	else if (GaussianBlurSize == 10) {	//Total 23 texture samples per fragment
										//Start with the center Guassian number. This is negative because we are including the center as 
										//the bilinear calculation below. It will be double counted if we don't first offset it by negative amount
		float4 color = -(G43TABS_CENTER * TEXTURE_0.Sample(TextureSampler0, PSInput.uv));
		//Add the other ones 
		[unroll(G43TABS_COUNT)]
		for (int i = 0; i < G43TABS_COUNT; ++i) {
			color += G43Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv - texOffset * G43Offsets[i]);
			color += G43Weights[i] * TEXTURE_0.Sample(TextureSampler0, PSInput.uv + texOffset * G43Offsets[i]);
		}
		//No need to divide by total weight as gaussianFilter adds up to 1.0 
		PSOutput.color = color;
	}
	//This should never happen ... 
	else {
		PSOutput.color = float4(1.0, 0.0, 0.0, 1.0);	//Red to indicate it's wrong
	}

#else

	//default to not blur anything
	PSOutput.color = TEXTURE_0.Sample(TextureSampler0, PSInput.uv);

#endif

}