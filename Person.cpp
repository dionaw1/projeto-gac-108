#include "Person.hpp"
#include <cstring>

Person::Person()
{
    id = -1;
    memset(personName, 0, sizeof(personName));
    memset(jobTitle, 0, sizeof(jobTitle));
    basePay = 0;
    overtimePay = 0;
    otherPay = 0;
    benefits = 0;
    totalPay = 0;
    totalPayBenefits = 0;
    year = 0;
}

Person::Person(int pId, const char *pName, const char *pJobTitle, double pBasePay, double pOvertimePay, double pOtherPay, double pBenefits, double pTotalPay, double pTotalPayBenefits, int pYear)
{
    id = pId;
    strncpy(personName, pName, MAX_NAME_LENGTH - 1);
    strncpy(jobTitle, pJobTitle, MAX_JOB_LENGTH - 1);
    basePay = pBasePay;
    overtimePay = pOvertimePay;
    otherPay = pOtherPay;
    benefits = pBenefits;
    totalPay = pTotalPay;
    totalPayBenefits = pTotalPayBenefits;
    year = pYear;
}