#include <iostream>
#include <string>
#include <bitset>

//from Crypto_CP4++.2019.pdf
//variant 7
//Popov Artem 
//

// L1 = x^30 + x^6 + x^4 + x + 1
// L2 = x^31 + x^3 + 1
// L3 = x^32 + x^7 + x^5 + x^3 + x^2 + x + 1
//
// (runtime for variant 7 is ~11 min 30 sec) 
// the worst case runtime is 15 min or 30 min or whatever
// 
// TO COMPILE : g++-8 Acr4.cpp -o Acr4.cpp -O3

//TODO test for another variants (var 14 is OK)

uint64_t* strToUint64(const std::string& ch_gamma,   const uint32_t N)
{
  
 
  uint64_t* bin_gamma = new uint64_t[N/64+1]{};//PAY ATTENTION TO THIS STUFF
  uint32_t count = 0;
  std::string tempStr;

  for (int i = 0; i <= N/64; ++i)
  {
    tempStr = ch_gamma.substr(count, 64);
    bin_gamma[i] = stoull(tempStr,0,  2);
    count +=64;
  }

  return bin_gamma;

}



void Geffe(uint64_t L1, uint64_t L2, uint64_t L3, uint64_t* bin_gamma,  const std::string& ch_gamma)
{
        
        uint64_t holder[320] = {};
        std::cout << "Geffe check" << std::endl;

        std::cout << "myGamma:" << std::endl;
        for(int i = 0; i < 320; ++i)
        {
                L1 = (L1 << 1) | (((L1 >> 29)^ (L1 >> 28) ^ (L1 >> 25) ^ (L1 >> 23)) & 1);
                L2 = (L2 << 1) | (((L2 >> 30)^ (L2 >> 27)) & 1);
                L3 = (L3 << 1) | (((L3 >> 31)^ (L3 >> 30) ^ (L3 >> 29) ^ (L3 >> 28) ^ (L3 >> 26) ^ (L3 >> 24)) & 1 );
              
                holder[i] = ( (((L3 >> 32) & 1 )*((L1 >> 30) & 1)) ^ ((((L3 >> 32) & 1) ^ 1) * ((L2 >> 31) & 1)) );
                std::cout << holder[i];
                if((i+1) % 64 == 0)
                {
                        std::cout << std::endl;
                }
        }
        std::cout << std::endl << "variant:" <<std::endl << std::bitset<64>(bin_gamma[0]) << std::endl << std::bitset<64>(bin_gamma[1]) << std::endl << std::bitset<64>(bin_gamma[2]) << std::endl <<std::bitset<64>(bin_gamma[3]) << std::endl << std::bitset<64>(bin_gamma[4]) << std::endl;


}

void L3_x32(uint64_t* bin_gamma, uint64_t pretenderL1[3000][5], uint64_t pretenderL2[3000][5], uint16_t capL1, uint16_t capL2, uint64_t& l1, uint64_t& l2, uint64_t& l3)//64 - 32 = 32 
{
        uint64_t* LSFR_state = new uint64_t[5]{};
        uint64_t step = 0;
        std::cout << "L3_x32 started" << std::endl;
        LSFR_state[0] = 0b1;
        for(int i = 0; i < 32; ++i)
        {

                LSFR_state[0] =  (LSFR_state[0]<< 1) | (((LSFR_state[0] >> 31)^(LSFR_state[0] >> 30)^(LSFR_state[0] >> 29)^(LSFR_state[0] >> 28) ^ (LSFR_state[0] >> 26) ^ (LSFR_state[0] >> 24)) & 1 );
                ++step;
        }
        
        for(int i = 1; i <5 ; ++i)//here we calculate initial state for massive LSFR_state
        {

        LSFR_state[i] = LSFR_state[i-1];
                for(int j = 0; j < 64; ++j)
                {
                LSFR_state[i] =  (LSFR_state[i]<< 1) | (((LSFR_state[i] >> 31)^(LSFR_state[i] >> 30)^(LSFR_state[i] >> 29)^(LSFR_state[i] >> 28) ^ (LSFR_state[i] >> 26) ^ (LSFR_state[i] >> 24)) & 1 );
                  ++step;
                }
        }
        //here we got the first initial state 00000000000000000000000000000001(32 bits) + 298 bits
 
        bool dropOther = false;
        uint16_t mgBit = 0; 
        uint16_t cand = 0;
        while(true) // well..
        {
                
                for(int pret1 = 0 ; pret1 < capL1; ++pret1)
                {
                   for(int pret2 = 0; pret2 < capL2; ++pret2)
                   {
                          for(int i = 0; i < 5;++i)
                          {
                                     for(int shift = 63; shift>= 0; --shift)
                                    {
                                 

                                 

                                mgBit = ((((LSFR_state[i] >> shift) & 1)*((pretenderL1[pret1][i] >> shift) & 1)) ^ ((((LSFR_state[i] >> shift) & 1) ^ 1) * ((pretenderL2[pret2][i] >> shift) & 1)));
                                        if(mgBit == ((bin_gamma[i] >> shift) & 1))
                                        {
                                              ++cand;
                                              if(cand == 300)
                                              {
                                                std::cout << "All initial states have been found on step #"<< step<<", no need to procede" << std::endl;
                                                l1 = pretenderL1[pret1][0];
                                                l2 = pretenderL2[pret2][0];
                                                l3 = LSFR_state[0];
                                                return;
                                              }
                                        }
                                        else
                                        {
                                                dropOther = true;
                                                break;
                                        }
                                   
                                   }
                                     if(dropOther == true)
                                     {
                                             break;
                                     }
                          }
                          cand = 0;
                          dropOther = false;
                    }
                }
                for(int i = 0; i < 4; ++i)
                {
                        LSFR_state[i] =( (LSFR_state[i] << 1) | (LSFR_state[i+1] >> 63) );//IS IT CORRECT AT ALL?
                }
                LSFR_state[4] =  (LSFR_state[4]<< 1) | (((LSFR_state[4] >> 31)^(LSFR_state[4] >> 30)^(LSFR_state[4] >> 29)^(LSFR_state[4] >> 28) ^ (LSFR_state[4] >> 26) ^ (LSFR_state[4] >> 24)) & 1 );
               ++step;

                if(step == 4294967295)
                {
                        std::cout << "LSFR ran into the initial state! No matches found  :("  << std::endl;
                        return;
                }
        }
              
}


void L2_x31(uint64_t* bin_gamma, uint64_t pretenderL2[3000][5], uint16_t& capL2)//64 - 31 = 33 
{
        uint64_t* LSFR_state = new uint64_t[5]{};

        LSFR_state[0] = 0b1;
        const uint64_t check = 0b0000000000000000000000000000000001111111111111111111111111111111; // 33 (0) + 31 (1)
        std::cout << "L2_x31 started" << std::endl;
        for(int i = 0; i < 33; ++i)
        {
                LSFR_state[0] = (LSFR_state[0] << 1) | (((LSFR_state[0] >> 30 )^(LSFR_state[0] >> 27)) & 1 );
        }
        
        for(int i = 1; i < 5; ++i)//here we calculate initial state for massive LSFR_state
        {
                LSFR_state[i] = LSFR_state[i-1];
                for(int j = 0; j < 64; ++j)
                {
                  LSFR_state[i] = (LSFR_state[i] << 1) | (((LSFR_state[i] >> 30 )^(LSFR_state[i] >> 27)) & 1 );
                }
        }
        //here we got the first initial state 0000000000000000000000000000001(31 bits) + 234 bits + 55 bits additional

                for(int i = 0; i < 4; ++i)//1 additional step 
                {
                        LSFR_state[i] =( (LSFR_state[i] << 1) | (LSFR_state[i+1] >> 63) );//IS IT CORRECT AT ALL?
                }
                LSFR_state[4] =  (LSFR_state[4]<< 1) | (((LSFR_state[4] >> 30 )^(LSFR_state[4] >> 27)) & 1 );
        uint32_t C = 0;
        uint64_t xr;
       
        
        while((LSFR_state[0] & check ) != 1)
        {
                C=0;
                for(int i = 0; i < 5; ++i)
                {
                    xr = bin_gamma[i] ^ LSFR_state[i];
                    C +=std::bitset<64>(xr).count();
                }
                if(C < 90 && capL2 < 3000)
                {
                        for(int i = 0; i < 5; ++i)
                        {
                                pretenderL2[capL2][i] = LSFR_state[i];
                        }
                        ++capL2;
                        std::cout << "PRETENDER l2 ALERT " << C << std::endl;
                        std::cout << "l2 : " << std::bitset<64>(LSFR_state[0]) <<std::endl;
                }

                //shift all parts of lsfr array << 1
                for(int i = 0; i < 4; ++i)
                {
                        LSFR_state[i] =( (LSFR_state[i] << 1) | (LSFR_state[i+1] >> 63) );//IS IT CORRECT AT ALL?
                }
                LSFR_state[4] =  (LSFR_state[4]<< 1) | (((LSFR_state[4] >> 30 )^(LSFR_state[4] >> 27)) & 1 );
              
        }
                        std::cout << "PRETENDERS AMOUNT " << capL2 << std::endl;
                        return;
}

void L1_x30(uint64_t* bin_gamma, uint64_t pretenderL1[3000][5], uint16_t& capL1)//64 - 30 = 34 
{
        uint64_t* LSFR_state = new uint64_t[5]{};

        std::cout << "L1_x30 started" << std::endl;
        LSFR_state[0] = 0b1;
        uint64_t check = 0b0000000000000000000000000000000000111111111111111111111111111111; // 34 (0) + 30 (1)
        for(int i = 0; i < 34; ++i)
        {
                LSFR_state[0] = (LSFR_state[0] << 1) | (((LSFR_state[0] >> 29 )^(LSFR_state[0] >> 28)^(LSFR_state[0] >> 25)^(LSFR_state[0] >> 23)) & 1 );
        }
        
        for(int i = 1; i < 5; ++i)//here we calculate initial state for massive LSFR_state
        {
                LSFR_state[i] = LSFR_state[i-1];
                for(int j = 0; j < 64; ++j)
                {
                  LSFR_state[i] = (LSFR_state[i] << 1) | (((LSFR_state[i] >> 29 )^(LSFR_state[i] >> 28)^(LSFR_state[i] >> 25)^(LSFR_state[i] >> 23)) & 1 );
                }
        }
        //here we got the first initial state 0000000000000000000000000000001(30 bits) + 228 bits + 52 bits additional

                for(int i = 0; i < 4; ++i)//1 additional step
                {
                        LSFR_state[i] =( (LSFR_state[i] << 1) | (LSFR_state[i+1] >> 63) );//IS IT CORRECT AT ALL?
                }
                  LSFR_state[4] = (LSFR_state[4] << 1) | (((LSFR_state[4] >> 29) ^ (LSFR_state[4] >> 28) ^ (LSFR_state[4] >> 25) ^ (LSFR_state[4] >> 23)) & 1);
        uint32_t C = 0;//N = 258, C = 81
        uint64_t xr;
       
        
        while((LSFR_state[0] &  check) != 1)
        {
                C = 0;
                for(int i = 0; i < 5; ++i)
                {
                    xr = bin_gamma[i] ^ LSFR_state[i];
                    C +=std::bitset<64>(xr).count();
                }
                if(C < 90  && capL1 < 3000)//mb add C >=60?
                {
                        for(int i = 0; i < 5; ++i)
                        {
                                pretenderL1[capL1][i] = LSFR_state[i];
                                
                        }
                        ++capL1;
                        std::cout << "PRETENDER l1 ALERT " << C << std::endl;
                        std::cout << "it is : " << std::bitset<64>(LSFR_state[0]) << std::endl;
                }

                //shift all parts of lsfr array << 1
                for(int i = 0; i < 4; ++i)
                {
                        LSFR_state[i] =( (LSFR_state[i] << 1) | (LSFR_state[i+1] >> 63) );//IS IT CORRECT AT ALL?
                }
                  LSFR_state[4] = (LSFR_state[4] << 1) | (((LSFR_state[4] >> 29) ^ (LSFR_state[4] >> 28) ^ (LSFR_state[4] >> 25) ^ (LSFR_state[4] >> 23)) & 1);
              
        }
                        std::cout << "PRETENDERS AMOUNT " << capL1 << std::endl;
                        return;
}






int main()
{
        std::string gamma = "00000101100001010101000110010100001101110001101000001011110011010001100100000011100101011111111110101101001000110100000010000110001010101001011101011001100100101111110100010101101011011111011101000101011001110111011011001101011001011110101101110000111000000011111001000000100011110101100001100101000101010000001010010000100100011100001001101110000001110000100111000101100110110101000010010001100001100001000111011001111001111100010011101011100000000010011010100110111010101100011101101101110110011010011000100010000010111010000110111010100101011000111100101110100110101110000110011011000110001101111101111101101001111100110001011011111000101000000111101011101000100010000011001110001000100010101100010110011111111100001000101000010001110111000111001110100100000111110110111101010001101110111100111100000111110000011000011110100100001101101100110111100110001011011111110000100111110001001100010101101001101101111001101101010011111110110010111111101011110000100101100101110111011101111101010011100100011011001010010011010001010111100101101100001100101001011101100101000010000000001010110110100110011010101011011000001111001000011100101011011010010111010111001100000111001100110011101110010101010100001010111110001000100010010111001001110110110100000110000111011100100111001101011010001110011011110110011111001111011000001010100100000101011010001111101001101111000100110111000100010001111101111110110011001000100011100110000100011100010111001010100111010010110110110010000111101000110011010111110010111110011110110001111110010101011011011100010101001101001101001010000000100110010111010111111100010110110001001101101110010001010001010001110100011000111011000011110101011100011110001001000001011010011110101101011101101101001100111100010010110010100101010100010110010100010111011000001000111111001010010011011010011011100000100101100010101111000100101001011111010110101110011000000101011100010001101010100001001111100000000011111011011011110000110010011110000010111111110111110010100101010011111011000000011011101011010111110000100011100111101111101111";
        const uint32_t N = 258;//258 + 55 

        

        uint16_t capL1 = 0,  capL2 = 0;
        uint64_t pretenderL1[3000][5] = {};
        uint64_t pretenderL2[3000][5] = {};

        uint64_t l1 = 0;// 0b0000010101000101010010011001010000000000000000000000000000000000;
        uint64_t l2 = 0;// 0b0010011110000101000100101110011000000000000000000000000000000000;
        uint64_t l3 = 0;// 0b1011101000100010110001111111001100000000000000000000000000000000;
        
         uint64_t* bin_gamma = strToUint64(gamma,  N);
        
      

        std::cout << "Variant 7 mask :" << std::endl;
        for(int i = 0 ; i <= N/64; ++i)
        {
               std::cout << std::bitset<64>(bin_gamma[i]) << std::endl;
        }
     
        L1_x30(bin_gamma, pretenderL1, capL1);
        L2_x31(bin_gamma, pretenderL2, capL2);
        L3_x32(bin_gamma, pretenderL1, pretenderL2, capL1, capL2, l1, l2, l3);
      
        l1 = l1 >> 34;
        l2 = l2 >> 33;
        l3 = l3 >> 32;

        std::cout << "Initial states : " << std::endl;
        std::cout << "L1_x30 : " <<  std::bitset<30>(l1) << std::endl;
        std::cout << "L2_x31 : " <<  std::bitset<31>(l2) << std::endl;
        std::cout << "L3_x32 : " <<  std::bitset<32>(l3) << std::endl;
        

        std::cout << std::endl;
        Geffe(l1, l2, l3, bin_gamma, gamma);
        std::cout << std::endl;
        delete[] bin_gamma;
        return 0;

}

