#ifndef PARAMS_H
#define PARAMS_H

struct PARAMS
{
	PARAMS()
		: alpha(1.0f), beta(1.0f),
		decreaseAlpha(false), decreaseBeta(false),
		alphaLimit(0.0f), betaLimit(0.0f),
		deltaAlpha(0.0f), deltaBeta(0.0f)
	{}

	float alpha;
	float beta;

	bool decreaseAlpha;
	bool decreaseBeta;

	float alphaLimit;
	float betaLimit;
	
	float deltaAlpha;
	float deltaBeta;
};


#endif