//#define MCLBN_FP_UNIT_SIZE 8
#include <cybozu/test.hpp>
#include <mcl/aggregate_sig.hpp>
#include <cybozu/benchmark.hpp>
#include <cybozu/xorshift.hpp>

using namespace mcl::aggs;

CYBOZU_TEST_AUTO(init)
{
	AGGS::init();
//	AGGS::init(mcl::bn::CurveFp382_1);
//	AGGS::init(mcl::bn::CurveFp462);
	SecretKey sec;
	sec.init();
	PublicKey pub;
	sec.getPublicKey(pub);
	const std::string m = "abc";
	Signature sig;
	sec.sign(sig, m);
	CYBOZU_TEST_ASSERT(pub.verify(sig, m));
}

void aggregateTest(const std::vector<std::string>& msgVec)
{
	const size_t n = msgVec.size();
	std::vector<SecretKey> secVec(n);
	std::vector<PublicKey> pubVec(n);
	std::vector<Signature> sigVec(n);
	Signature aggSig;
	for (size_t i = 0; i < n; i++) {
		secVec[i].init();
		secVec[i].getPublicKey(pubVec[i]);
		secVec[i].sign(sigVec[i], msgVec[i]);
		CYBOZU_TEST_ASSERT(pubVec[i].verify(sigVec[i], msgVec[i]));
	}
	aggSig.aggregate(sigVec);
	CYBOZU_TEST_ASSERT(aggSig.verify(msgVec, pubVec));
	CYBOZU_BENCH_C("aggSig.verify", 10, aggSig.verify, msgVec, pubVec);
}

CYBOZU_TEST_AUTO(aggregate)
{
#if 0
	/*
		Core i7-7700 CPU @ 3.60GHz
		              BN254  Fp382   Fp462
		security bit   100    115?     128
		# of sig 100    69     200     476
		        1000   693    2037    4731
		       10000  6969   20448   47000(Mclk)
	*/
	const size_t n = 1000;
	const size_t msgSize = 16;
	std::vector<std::string> msgVec(n);
	cybozu::XorShift rg;
	for (size_t i = 0; i < n; i++) {
		std::string& msg = msgVec[i];
		msg.resize(msgSize);
		for (size_t j = 0; j < msgSize; j++) {
			msg[j] = (char)rg();
		}
	}
	aggregateTest(msgVec);
#else
	const std::string msgArray[] = { "abc", "12345", "xyz", "pqr", "aggregate signature" };
	const size_t n = sizeof(msgArray) / sizeof(msgArray[0]);
	std::vector<std::string> msgVec(n);
	for (size_t i = 0; i < n; i++) {
		msgVec[i] = msgArray[i];
	}
	aggregateTest(msgVec);
#endif
}
