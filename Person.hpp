#ifndef PERSON_H
#define PERSON_H

const int MAX_NAME_LENGTH = 50;
const int MAX_JOB_LENGTH = 50;

struct Person {
    int id;
    char personName[MAX_NAME_LENGTH];
    char jobTitle[MAX_JOB_LENGTH];
    double basePay;
    double overtimePay;
    double otherPay;
    double benefits;
    double totalPay;
    double totalPayBenefits;
    int year;

    Person();
    Person(int pId, const char *pName, const char *pJobTitle, double pBasePay, double pOvertimePay, double pOtherPay, double pBenefits, double pTotalPay, double pTotalPayBenefits, int pYear);
};

#endif
