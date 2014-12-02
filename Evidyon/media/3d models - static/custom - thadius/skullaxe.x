xof 0303txt 0032


template VertexDuplicationIndices { 
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}
template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}
template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array float weights[nWeights];
 Matrix4x4 matrixOffset;
}

Frame RootFrame {

  FrameTransformMatrix {
    1.000000,0.000000,0.000000,0.000000,
    0.000000,1.000000,0.000000,0.000000,
    0.000000,0.000000,-1.000000,0.000000,
    0.000000,0.000000,0.000000,1.000000;;
  }
  Frame Cube {

    FrameTransformMatrix {
      1.000000,0.000000,0.000000,0.000000,
      0.000000,1.000000,0.000000,0.000000,
      0.000000,0.000000,1.000000,0.000000,
      0.000000,0.000000,1.800000,1.000000;;
    }
Mesh {
352;
-0.311300; -0.180000; 2.347700;,
-0.158300; -0.180000; 2.347700;,
-0.158300; -0.180000; 2.647700;,
-0.311300; -0.180000; 2.647700;,
-0.158300; -0.180000; 2.347700;,
-0.011300; -0.180000; 2.347700;,
-0.011300; -0.180000; 2.647700;,
-0.158300; -0.180000; 2.647700;,
-0.311300; -0.180000; 2.047700;,
-0.158300; -0.180000; 2.047700;,
-0.158300; -0.180000; 2.347700;,
-0.311300; -0.180000; 2.347700;,
-0.158300; -0.180000; 2.047700;,
-0.011300; -0.180000; 2.047700;,
-0.011300; -0.180000; 2.347700;,
-0.158300; -0.180000; 2.347700;,
-0.311300; 0.180000; 2.347700;,
-0.158300; 0.180000; 2.347700;,
-0.158300; 0.180000; 2.047700;,
-0.311300; 0.180000; 2.047700;,
-0.158300; 0.180000; 2.347700;,
-0.011300; 0.180000; 2.347700;,
-0.011300; 0.180000; 2.047700;,
-0.158300; 0.180000; 2.047700;,
-0.311300; 0.180000; 2.647700;,
-0.158300; 0.180000; 2.647700;,
-0.158300; 0.180000; 2.347700;,
-0.311300; 0.180000; 2.347700;,
-0.158300; 0.180000; 2.647700;,
-0.011300; 0.180000; 2.647700;,
-0.011300; 0.180000; 2.347700;,
-0.158300; 0.180000; 2.347700;,
-0.011300; -0.180000; 2.347700;,
0.138700; -0.180000; 2.347700;,
0.138700; -0.180000; 2.647700;,
-0.011300; -0.180000; 2.647700;,
0.138700; -0.180000; 2.347700;,
0.288700; -0.180000; 2.347700;,
0.288700; -0.180000; 2.647700;,
0.138700; -0.180000; 2.647700;,
-0.011300; -0.180000; 2.047700;,
0.138700; -0.180000; 2.047700;,
0.138700; -0.180000; 2.347700;,
-0.011300; -0.180000; 2.347700;,
0.138700; -0.180000; 2.047700;,
0.288700; -0.180000; 2.047700;,
0.288700; -0.180000; 2.347700;,
0.138700; -0.180000; 2.347700;,
-0.011300; 0.180000; 2.347700;,
0.138700; 0.180000; 2.347700;,
0.138700; 0.180000; 2.047700;,
-0.011300; 0.180000; 2.047700;,
0.288700; 0.180000; 2.047700;,
0.138700; 0.180000; 2.047700;,
0.138700; 0.180000; 2.347700;,
0.288700; 0.180000; 2.347700;,
-0.011300; 0.180000; 2.647700;,
0.138700; 0.180000; 2.647700;,
0.138700; 0.180000; 2.347700;,
-0.011300; 0.180000; 2.347700;,
0.138700; 0.180000; 2.647700;,
0.288700; 0.180000; 2.647700;,
0.288700; 0.180000; 2.347700;,
0.138700; 0.180000; 2.347700;,
0.288700; -0.180000; 2.347700;,
1.061200; -0.150000; 2.347700;,
0.739400; -0.150000; 3.229700;,
0.288700; -0.180000; 2.647700;,
1.061200; -0.150000; 2.347700;,
1.704700; 0.000000; 2.347700;,
1.061200; 0.000000; 3.607700;,
0.739400; -0.150000; 3.229700;,
0.288700; -0.180000; 2.047700;,
0.739400; -0.150000; 1.465700;,
1.061200; -0.150000; 2.347700;,
0.288700; -0.180000; 2.347700;,
0.739400; -0.150000; 1.465700;,
1.061200; 0.000000; 1.087700;,
1.704700; 0.000000; 2.347700;,
1.061200; -0.150000; 2.347700;,
-1.727300; 0.000000; 2.347700;,
-1.083800; -0.150000; 2.347700;,
-0.762100; -0.150000; 3.229700;,
-1.083800; 0.000000; 3.607700;,
-1.083800; -0.150000; 2.347700;,
-0.311300; -0.180000; 2.347700;,
-0.311300; -0.180000; 2.647700;,
-0.762100; -0.150000; 3.229700;,
-1.083800; 0.000000; 1.087700;,
-0.762100; -0.150000; 1.465700;,
-1.083800; -0.150000; 2.347700;,
-1.727300; 0.000000; 2.347700;,
-0.762100; -0.150000; 1.465700;,
-0.311300; -0.180000; 2.047700;,
-0.311300; -0.180000; 2.347700;,
-1.083800; -0.150000; 2.347700;,
-0.158300; -0.180000; 2.047700;,
-0.157200; -0.150000; 1.814400;,
-0.011300; -0.150000; 1.837700;,
-0.011300; -0.180000; 2.047700;,
-0.311300; -0.180000; 2.047700;,
-0.762100; -0.150000; 1.465700;,
-0.157200; -0.150000; 1.814400;,
-0.158300; -0.180000; 2.047700;,
-0.762100; -0.150000; 1.465700;,
-1.083800; 0.000000; 1.087700;,
-0.156100; 0.000000; 1.827200;,
-0.157200; -0.150000; 1.814400;,
-0.158300; -0.180000; 2.647700;,
-0.157200; -0.150000; 2.881100;,
-0.762100; -0.150000; 3.229700;,
-0.311300; -0.180000; 2.647700;,
-0.157200; -0.150000; 2.881100;,
-0.156100; 0.000000; 2.868200;,
-1.083800; 0.000000; 3.607700;,
-0.762100; -0.150000; 3.229700;,
-0.011300; -0.180000; 2.647700;,
-0.011300; -0.150000; 2.857700;,
-0.157200; -0.150000; 2.881100;,
-0.158300; -0.180000; 2.647700;,
0.138700; -0.180000; 2.047700;,
0.139800; -0.150000; 1.811500;,
0.739400; -0.150000; 1.465700;,
0.288700; -0.180000; 2.047700;,
0.139800; -0.150000; 1.811500;,
0.140900; 0.000000; 1.822700;,
1.061200; 0.000000; 1.087700;,
0.739400; -0.150000; 1.465700;,
-0.011300; -0.180000; 2.047700;,
-0.011300; -0.150000; 1.837700;,
0.139800; -0.150000; 1.811500;,
0.138700; -0.180000; 2.047700;,
0.138700; -0.180000; 2.647700;,
0.139800; -0.150000; 2.884000;,
-0.011300; -0.150000; 2.857700;,
-0.011300; -0.180000; 2.647700;,
0.288700; -0.180000; 2.647700;,
0.739400; -0.150000; 3.229700;,
0.139800; -0.150000; 2.884000;,
0.138700; -0.180000; 2.647700;,
0.739400; -0.150000; 3.229700;,
1.061200; 0.000000; 3.607700;,
0.140900; 0.000000; 2.872700;,
0.139800; -0.150000; 2.884000;,
1.704700; 0.000000; 2.347700;,
1.061200; 0.150000; 2.347700;,
0.739400; 0.150000; 3.229700;,
1.061200; 0.000000; 3.607700;,
1.061200; 0.150000; 2.347700;,
0.288700; 0.180000; 2.347700;,
0.288700; 0.180000; 2.647700;,
0.739400; 0.150000; 3.229700;,
1.061200; 0.000000; 1.087700;,
0.739400; 0.150000; 1.465700;,
1.061200; 0.150000; 2.347700;,
1.704700; 0.000000; 2.347700;,
0.739400; 0.150000; 1.465700;,
0.288700; 0.180000; 2.047700;,
0.288700; 0.180000; 2.347700;,
1.061200; 0.150000; 2.347700;,
-0.311300; 0.180000; 2.347700;,
-1.083800; 0.150000; 2.347700;,
-0.762100; 0.150000; 3.229700;,
-0.311300; 0.180000; 2.647700;,
-1.083800; 0.150000; 2.347700;,
-1.727300; 0.000000; 2.347700;,
-1.083800; 0.000000; 3.607700;,
-0.762100; 0.150000; 3.229700;,
-0.311300; 0.180000; 2.047700;,
-0.762100; 0.150000; 1.465700;,
-1.083800; 0.150000; 2.347700;,
-0.311300; 0.180000; 2.347700;,
-0.762100; 0.150000; 1.465700;,
-1.083800; 0.000000; 1.087700;,
-1.727300; 0.000000; 2.347700;,
-1.083800; 0.150000; 2.347700;,
-0.157200; 0.150000; 1.814400;,
-0.158300; 0.180000; 2.047700;,
-0.011300; 0.180000; 2.047700;,
-0.011300; 0.150000; 1.837700;,
-1.083800; 0.000000; 1.087700;,
-0.762100; 0.150000; 1.465700;,
-0.157200; 0.150000; 1.814400;,
-0.156100; 0.000000; 1.827200;,
-0.762100; 0.150000; 1.465700;,
-0.311300; 0.180000; 2.047700;,
-0.158300; 0.180000; 2.047700;,
-0.157200; 0.150000; 1.814400;,
-0.156100; 0.000000; 2.868200;,
-0.157200; 0.150000; 2.881100;,
-0.762100; 0.150000; 3.229700;,
-1.083800; 0.000000; 3.607700;,
-0.157200; 0.150000; 2.881100;,
-0.158300; 0.180000; 2.647700;,
-0.311300; 0.180000; 2.647700;,
-0.762100; 0.150000; 3.229700;,
-0.011300; 0.150000; 2.857700;,
-0.011300; 0.180000; 2.647700;,
-0.158300; 0.180000; 2.647700;,
-0.157200; 0.150000; 2.881100;,
0.140900; 0.000000; 1.822700;,
0.139800; 0.150000; 1.811500;,
0.739400; 0.150000; 1.465700;,
1.061200; 0.000000; 1.087700;,
0.288700; 0.180000; 2.047700;,
0.739400; 0.150000; 1.465700;,
0.139800; 0.150000; 1.811500;,
0.138700; 0.180000; 2.047700;,
-0.011300; 0.150000; 1.837700;,
-0.011300; 0.180000; 2.047700;,
0.138700; 0.180000; 2.047700;,
0.139800; 0.150000; 1.811500;,
0.139800; 0.150000; 2.884000;,
0.138700; 0.180000; 2.647700;,
-0.011300; 0.180000; 2.647700;,
-0.011300; 0.150000; 2.857700;,
1.061200; 0.000000; 3.607700;,
0.739400; 0.150000; 3.229700;,
0.139800; 0.150000; 2.884000;,
0.140900; 0.000000; 2.872700;,
0.739400; 0.150000; 3.229700;,
0.288700; 0.180000; 2.647700;,
0.138700; 0.180000; 2.647700;,
0.139800; 0.150000; 2.884000;,
0.139800; 0.150000; 2.884000;,
-0.011300; 0.150000; 2.857700;,
-0.014200; 0.150000; 3.157700;,
0.136900; 0.150000; 3.184000;,
-0.011300; 0.150000; 2.857700;,
-0.157200; 0.150000; 2.881100;,
-0.160100; 0.150000; 3.181000;,
-0.014200; 0.150000; 3.157700;,
0.140900; 0.000000; 2.872700;,
0.139800; 0.150000; 2.884000;,
0.136900; 0.150000; 3.184000;,
0.138000; 0.000000; 3.172700;,
-0.157200; 0.150000; 2.881100;,
-0.156100; 0.000000; 2.868200;,
-0.159000; 0.000000; 3.168200;,
-0.160100; 0.150000; 3.181000;,
-0.011300; -0.150000; 2.857700;,
0.139800; -0.150000; 2.884000;,
0.136900; -0.150000; 3.184000;,
-0.014200; -0.150000; 3.157700;,
-0.157200; -0.150000; 2.881100;,
-0.011300; -0.150000; 2.857700;,
-0.014200; -0.150000; 3.157700;,
-0.160100; -0.150000; 3.181000;,
0.139800; -0.150000; 2.884000;,
0.140900; 0.000000; 2.872700;,
0.138000; 0.000000; 3.172700;,
0.136900; -0.150000; 3.184000;,
-0.156100; 0.000000; 2.868200;,
-0.157200; -0.150000; 2.881100;,
-0.160100; -0.150000; 3.181000;,
-0.159000; 0.000000; 3.168200;,
-0.014200; -0.150000; 3.157700;,
-0.014200; 0.000000; 3.157700;,
-0.159000; 0.000000; 3.168200;,
-0.160100; -0.150000; 3.181000;,
0.136900; -0.150000; 3.184000;,
0.138000; 0.000000; 3.172700;,
-0.014200; 0.000000; 3.157700;,
-0.014200; -0.150000; 3.157700;,
-0.014200; 0.000000; 3.157700;,
-0.014200; 0.150000; 3.157700;,
-0.160100; 0.150000; 3.181000;,
-0.159000; 0.000000; 3.168200;,
0.138000; 0.000000; 3.172700;,
0.136900; 0.150000; 3.184000;,
-0.014200; 0.150000; 3.157700;,
-0.014200; 0.000000; 3.157700;,
-0.011300; 0.150000; 1.837700;,
0.139800; 0.150000; 1.811500;,
0.110900; 0.150000; -3.288400;,
-0.040300; 0.150000; -3.262100;,
-0.157200; 0.150000; 1.814400;,
-0.011300; 0.150000; 1.837700;,
-0.040300; 0.150000; -3.262100;,
-0.186100; 0.150000; -3.285500;,
0.139800; 0.150000; 1.811500;,
0.140900; 0.000000; 1.822700;,
0.112000; 0.000000; -3.277100;,
0.110900; 0.150000; -3.288400;,
-0.156100; 0.000000; 1.827200;,
-0.157200; 0.150000; 1.814400;,
-0.186100; 0.150000; -3.285500;,
-0.185000; 0.000000; -3.272600;,
0.139800; -0.150000; 1.811500;,
-0.011300; -0.150000; 1.837700;,
-0.040300; -0.150000; -3.262100;,
0.110900; -0.150000; -3.288400;,
-0.011300; -0.150000; 1.837700;,
-0.157200; -0.150000; 1.814400;,
-0.186100; -0.150000; -3.285500;,
-0.040300; -0.150000; -3.262100;,
0.140900; 0.000000; 1.822700;,
0.139800; -0.150000; 1.811500;,
0.110900; -0.150000; -3.288400;,
0.112000; 0.000000; -3.277100;,
-0.157200; -0.150000; 1.814400;,
-0.156100; 0.000000; 1.827200;,
-0.185000; 0.000000; -3.272600;,
-0.186100; -0.150000; -3.285500;,
-0.186100; -0.150000; -3.285500;,
-0.185000; 0.000000; -3.272600;,
-0.187900; 0.000000; -3.572600;,
-0.189000; -0.150000; -3.585400;,
0.112000; 0.000000; -3.277100;,
0.110900; -0.150000; -3.288400;,
0.108000; -0.150000; -3.588400;,
0.109100; 0.000000; -3.577100;,
-0.040300; -0.150000; -3.262100;,
-0.186100; -0.150000; -3.285500;,
-0.189000; -0.150000; -3.585400;,
-0.043100; -0.150000; -3.562100;,
0.110900; -0.150000; -3.288400;,
-0.040300; -0.150000; -3.262100;,
-0.043100; -0.150000; -3.562100;,
0.108000; -0.150000; -3.588400;,
-0.185000; 0.000000; -3.272600;,
-0.186100; 0.150000; -3.285500;,
-0.189000; 0.150000; -3.585400;,
-0.187900; 0.000000; -3.572600;,
0.110900; 0.150000; -3.288400;,
0.112000; 0.000000; -3.277100;,
0.109100; 0.000000; -3.577100;,
0.108000; 0.150000; -3.588400;,
-0.186100; 0.150000; -3.285500;,
-0.040300; 0.150000; -3.262100;,
-0.043100; 0.150000; -3.562100;,
-0.189000; 0.150000; -3.585400;,
-0.040300; 0.150000; -3.262100;,
0.110900; 0.150000; -3.288400;,
0.108000; 0.150000; -3.588400;,
-0.043100; 0.150000; -3.562100;,
-0.189000; -0.150000; -3.585400;,
-0.187900; 0.000000; -3.572600;,
-0.043100; 0.000000; -3.562100;,
-0.043100; -0.150000; -3.562100;,
-0.043100; -0.150000; -3.562100;,
-0.043100; 0.000000; -3.562100;,
0.109100; 0.000000; -3.577100;,
0.108000; -0.150000; -3.588400;,
-0.187900; 0.000000; -3.572600;,
-0.189000; 0.150000; -3.585400;,
-0.043100; 0.150000; -3.562100;,
-0.043100; 0.000000; -3.562100;,
-0.043100; 0.000000; -3.562100;,
-0.043100; 0.150000; -3.562100;,
0.108000; 0.150000; -3.588400;,
0.109100; 0.000000; -3.577100;;
88;
4; 0, 3, 2, 1;,
4; 4, 7, 6, 5;,
4; 8, 11, 10, 9;,
4; 12, 15, 14, 13;,
4; 16, 19, 18, 17;,
4; 20, 23, 22, 21;,
4; 24, 27, 26, 25;,
4; 28, 31, 30, 29;,
4; 32, 35, 34, 33;,
4; 36, 39, 38, 37;,
4; 40, 43, 42, 41;,
4; 44, 47, 46, 45;,
4; 48, 51, 50, 49;,
4; 52, 55, 54, 53;,
4; 56, 59, 58, 57;,
4; 60, 63, 62, 61;,
4; 64, 67, 66, 65;,
4; 68, 71, 70, 69;,
4; 72, 75, 74, 73;,
4; 76, 79, 78, 77;,
4; 80, 83, 82, 81;,
4; 84, 87, 86, 85;,
4; 88, 91, 90, 89;,
4; 92, 95, 94, 93;,
4; 96, 99, 98, 97;,
4; 100, 103, 102, 101;,
4; 104, 107, 106, 105;,
4; 108, 111, 110, 109;,
4; 112, 115, 114, 113;,
4; 116, 119, 118, 117;,
4; 120, 123, 122, 121;,
4; 124, 127, 126, 125;,
4; 128, 131, 130, 129;,
4; 132, 135, 134, 133;,
4; 136, 139, 138, 137;,
4; 140, 143, 142, 141;,
4; 144, 147, 146, 145;,
4; 148, 151, 150, 149;,
4; 152, 155, 154, 153;,
4; 156, 159, 158, 157;,
4; 160, 163, 162, 161;,
4; 164, 167, 166, 165;,
4; 168, 171, 170, 169;,
4; 172, 175, 174, 173;,
4; 176, 179, 178, 177;,
4; 180, 183, 182, 181;,
4; 184, 187, 186, 185;,
4; 188, 191, 190, 189;,
4; 192, 195, 194, 193;,
4; 196, 199, 198, 197;,
4; 200, 203, 202, 201;,
4; 204, 207, 206, 205;,
4; 208, 211, 210, 209;,
4; 212, 215, 214, 213;,
4; 216, 219, 218, 217;,
4; 220, 223, 222, 221;,
4; 224, 227, 226, 225;,
4; 228, 231, 230, 229;,
4; 232, 235, 234, 233;,
4; 236, 239, 238, 237;,
4; 240, 243, 242, 241;,
4; 244, 247, 246, 245;,
4; 248, 251, 250, 249;,
4; 252, 255, 254, 253;,
4; 256, 259, 258, 257;,
4; 260, 263, 262, 261;,
4; 264, 267, 266, 265;,
4; 268, 271, 270, 269;,
4; 272, 275, 274, 273;,
4; 276, 279, 278, 277;,
4; 280, 283, 282, 281;,
4; 284, 287, 286, 285;,
4; 288, 291, 290, 289;,
4; 292, 295, 294, 293;,
4; 296, 299, 298, 297;,
4; 300, 303, 302, 301;,
4; 304, 307, 306, 305;,
4; 308, 311, 310, 309;,
4; 312, 315, 314, 313;,
4; 316, 319, 318, 317;,
4; 320, 323, 322, 321;,
4; 324, 327, 326, 325;,
4; 328, 331, 330, 329;,
4; 332, 335, 334, 333;,
4; 336, 339, 338, 337;,
4; 340, 343, 342, 341;,
4; 344, 347, 346, 345;,
4; 348, 351, 350, 349;;
  MeshMaterialList {
    3;
    88;
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1;;
  Material Material {
    0.800000; 0.800000; 0.800000;1.0;;
    0.144621;
    1.000000; 1.000000; 1.000000;;
    0.0; 0.0; 0.0;;
  }  //End of Material
  Material Mat2 {
    1.0; 1.0; 1.0; 1.0;;
    1.0;
    1.0; 1.0; 1.0;;
    0.0; 0.0; 0.0;;
  TextureFilename {    "skullwall.png";  }
  }  // End of Material
  Material Mat2 {
    1.0; 1.0; 1.0; 1.0;;
    1.0;
    1.0; 1.0; 1.0;;
    0.0; 0.0; 0.0;;
  TextureFilename {    "crate.jpg";  }
  }  // End of Material
    }  //End of MeshMaterialList
  MeshNormals {
352;
    -0.021668; -0.999756; 0.000000;,
    0.000000; -1.000000; 0.000000;,
    0.013947; -0.998199; 0.058138;,
    0.000549; -0.999298; 0.036653;,
    0.000000; -1.000000; 0.000000;,
    0.000000; -1.000000; 0.000000;,
    -0.000214; -0.997742; 0.066988;,
    0.013947; -0.998199; 0.058138;,
    0.000549; -0.999298; -0.036653;,
    0.013947; -0.998199; -0.058138;,
    0.000000; -1.000000; 0.000000;,
    -0.021668; -0.999756; 0.000000;,
    0.013947; -0.998199; -0.058138;,
    -0.000214; -0.997742; -0.066988;,
    0.000000; -1.000000; 0.000000;,
    0.000000; -1.000000; 0.000000;,
    -0.021668; 0.999756; 0.000000;,
    0.000000; 1.000000; 0.000000;,
    0.013947; 0.998199; -0.058138;,
    0.000549; 0.999298; -0.036653;,
    0.000000; 1.000000; 0.000000;,
    0.000000; 1.000000; 0.000000;,
    -0.000214; 0.997742; -0.066988;,
    0.013947; 0.998199; -0.058138;,
    0.000549; 0.999298; 0.036653;,
    0.013947; 0.998199; 0.058138;,
    0.000000; 1.000000; 0.000000;,
    -0.021668; 0.999756; 0.000000;,
    0.013947; 0.998199; 0.058138;,
    -0.000214; 0.997742; 0.066988;,
    0.000000; 1.000000; 0.000000;,
    0.000000; 1.000000; 0.000000;,
    0.000000; -1.000000; 0.000000;,
    0.000000; -1.000000; 0.000000;,
    -0.014191; -0.998199; 0.057863;,
    -0.000214; -0.997742; 0.066988;,
    0.000000; -1.000000; 0.000000;,
    0.021668; -0.999756; 0.000000;,
    -0.000549; -0.999329; 0.036592;,
    -0.014191; -0.998199; 0.057863;,
    -0.000214; -0.997742; -0.066988;,
    -0.014191; -0.998199; -0.057863;,
    0.000000; -1.000000; 0.000000;,
    0.000000; -1.000000; 0.000000;,
    -0.014191; -0.998199; -0.057863;,
    -0.000549; -0.999329; -0.036592;,
    0.021668; -0.999756; 0.000000;,
    0.000000; -1.000000; 0.000000;,
    0.000000; 1.000000; 0.000000;,
    0.000000; 1.000000; 0.000000;,
    -0.014191; 0.998199; -0.057863;,
    -0.000214; 0.997742; -0.066988;,
    -0.000549; 0.999329; -0.036592;,
    -0.014191; 0.998199; -0.057863;,
    0.000000; 1.000000; 0.000000;,
    0.021668; 0.999756; 0.000000;,
    -0.000214; 0.997742; 0.066988;,
    -0.014191; 0.998199; 0.057863;,
    0.000000; 1.000000; 0.000000;,
    0.000000; 1.000000; 0.000000;,
    -0.014191; 0.998199; 0.057863;,
    -0.000549; 0.999329; 0.036592;,
    0.021668; 0.999756; 0.000000;,
    0.000000; 1.000000; 0.000000;,
    0.021668; -0.999756; 0.000000;,
    0.149907; -0.988678; 0.000000;,
    -0.084109; -0.954680; 0.285470;,
    -0.000549; -0.999329; 0.036592;,
    0.149907; -0.988678; 0.000000;,
    1.000000; 0.000000; 0.000000;,
    -0.312906; 0.000000; 0.949767;,
    -0.084109; -0.954680; 0.285470;,
    -0.000549; -0.999329; -0.036592;,
    -0.084109; -0.954680; -0.285470;,
    0.149907; -0.988678; 0.000000;,
    0.021668; -0.999756; 0.000000;,
    -0.084109; -0.954680; -0.285470;,
    -0.312906; 0.000000; -0.949767;,
    1.000000; 0.000000; 0.000000;,
    0.149907; -0.988678; 0.000000;,
    -1.000000; 0.000000; 0.000000;,
    -0.149907; -0.988678; 0.000000;,
    0.083865; -0.954680; 0.285470;,
    0.312143; 0.000000; 0.950011;,
    -0.149907; -0.988678; 0.000000;,
    -0.021668; -0.999756; 0.000000;,
    0.000549; -0.999298; 0.036653;,
    0.083865; -0.954680; 0.285470;,
    0.312143; 0.000000; -0.950011;,
    0.083865; -0.954680; -0.285470;,
    -0.149907; -0.988678; 0.000000;,
    -1.000000; 0.000000; 0.000000;,
    0.083865; -0.954680; -0.285470;,
    0.000549; -0.999298; -0.036653;,
    -0.021668; -0.999756; 0.000000;,
    -0.149907; -0.988678; 0.000000;,
    0.013947; -0.998199; -0.058138;,
    -0.114963; -0.952116; -0.283273;,
    -0.000214; -0.997742; -0.066988;,
    -0.000214; -0.997742; -0.066988;,
    0.000549; -0.999298; -0.036653;,
    0.083865; -0.954680; -0.285470;,
    -0.114963; -0.952116; -0.283273;,
    0.013947; -0.998199; -0.058138;,
    0.083865; -0.954680; -0.285470;,
    0.312143; 0.000000; -0.950011;,
    -0.516831; 0.000000; -0.856075;,
    -0.114963; -0.952116; -0.283273;,
    0.013947; -0.998199; 0.058138;,
    -0.114963; -0.952422; 0.282174;,
    0.083865; -0.954680; 0.285470;,
    0.000549; -0.999298; 0.036653;,
    -0.114963; -0.952422; 0.282174;,
    -0.518784; 0.000000; 0.854885;,
    0.312143; 0.000000; 0.950011;,
    0.083865; -0.954680; 0.285470;,
    -0.000214; -0.997742; 0.066988;,
    -0.000214; -0.997742; 0.066988;,
    -0.114963; -0.952422; 0.282174;,
    0.013947; -0.998199; 0.058138;,
    -0.014191; -0.998199; -0.057863;,
    0.113956; -0.951720; -0.284951;,
    -0.084109; -0.954680; -0.285470;,
    -0.000549; -0.999329; -0.036592;,
    0.113956; -0.951720; -0.284951;,
    0.510636; 0.000000; -0.859767;,
    -0.312906; 0.000000; -0.949767;,
    -0.084109; -0.954680; -0.285470;,
    -0.000214; -0.997742; -0.066988;,
    -0.000214; -0.997742; -0.066988;,
    0.113956; -0.951720; -0.284951;,
    -0.014191; -0.998199; -0.057863;,
    -0.014191; -0.998199; 0.057863;,
    0.113926; -0.951415; 0.285989;,
    -0.000214; -0.997742; 0.066988;,
    -0.000214; -0.997742; 0.066988;,
    -0.000549; -0.999329; 0.036592;,
    -0.084109; -0.954680; 0.285470;,
    0.113926; -0.951415; 0.285989;,
    -0.014191; -0.998199; 0.057863;,
    -0.084109; -0.954680; 0.285470;,
    -0.312906; 0.000000; 0.949767;,
    0.508652; 0.000000; 0.860958;,
    0.113926; -0.951415; 0.285989;,
    1.000000; 0.000000; 0.000000;,
    0.149907; 0.988678; 0.000000;,
    -0.084109; 0.954680; 0.285470;,
    -0.312906; 0.000000; 0.949767;,
    0.149907; 0.988678; 0.000000;,
    0.021668; 0.999756; 0.000000;,
    -0.000549; 0.999329; 0.036592;,
    -0.084109; 0.954680; 0.285470;,
    -0.312906; 0.000000; -0.949767;,
    -0.084109; 0.954680; -0.285470;,
    0.149907; 0.988678; 0.000000;,
    1.000000; 0.000000; 0.000000;,
    -0.084109; 0.954680; -0.285470;,
    -0.000549; 0.999329; -0.036592;,
    0.021668; 0.999756; 0.000000;,
    0.149907; 0.988678; 0.000000;,
    -0.021668; 0.999756; 0.000000;,
    -0.149907; 0.988678; 0.000000;,
    0.083865; 0.954680; 0.285470;,
    0.000549; 0.999298; 0.036653;,
    -0.149907; 0.988678; 0.000000;,
    -1.000000; 0.000000; 0.000000;,
    0.312143; 0.000000; 0.950011;,
    0.083865; 0.954680; 0.285470;,
    0.000549; 0.999298; -0.036653;,
    0.083865; 0.954680; -0.285470;,
    -0.149907; 0.988678; 0.000000;,
    -0.021668; 0.999756; 0.000000;,
    0.083865; 0.954680; -0.285470;,
    0.312143; 0.000000; -0.950011;,
    -1.000000; 0.000000; 0.000000;,
    -0.149907; 0.988678; 0.000000;,
    -0.114963; 0.952116; -0.283273;,
    0.013947; 0.998199; -0.058138;,
    -0.000214; 0.997742; -0.066988;,
    -0.000214; 0.997742; -0.066988;,
    0.312143; 0.000000; -0.950011;,
    0.083865; 0.954680; -0.285470;,
    -0.114963; 0.952116; -0.283273;,
    -0.516831; 0.000000; -0.856075;,
    0.083865; 0.954680; -0.285470;,
    0.000549; 0.999298; -0.036653;,
    0.013947; 0.998199; -0.058138;,
    -0.114963; 0.952116; -0.283273;,
    -0.518784; 0.000000; 0.854885;,
    -0.114963; 0.952422; 0.282174;,
    0.083865; 0.954680; 0.285470;,
    0.312143; 0.000000; 0.950011;,
    -0.114963; 0.952422; 0.282174;,
    0.013947; 0.998199; 0.058138;,
    0.000549; 0.999298; 0.036653;,
    0.083865; 0.954680; 0.285470;,
    -0.000214; 0.997742; 0.066988;,
    -0.000214; 0.997742; 0.066988;,
    0.013947; 0.998199; 0.058138;,
    -0.114963; 0.952422; 0.282174;,
    0.510636; 0.000000; -0.859767;,
    0.113987; 0.951720; -0.284951;,
    -0.084109; 0.954680; -0.285470;,
    -0.312906; 0.000000; -0.949767;,
    -0.000549; 0.999329; -0.036592;,
    -0.084109; 0.954680; -0.285470;,
    0.113987; 0.951720; -0.284951;,
    -0.014191; 0.998199; -0.057863;,
    -0.000214; 0.997742; -0.066988;,
    -0.000214; 0.997742; -0.066988;,
    -0.014191; 0.998199; -0.057863;,
    0.113987; 0.951720; -0.284951;,
    0.113926; 0.951415; 0.286019;,
    -0.014191; 0.998199; 0.057863;,
    -0.000214; 0.997742; 0.066988;,
    -0.000214; 0.997742; 0.066988;,
    -0.312906; 0.000000; 0.949767;,
    -0.084109; 0.954680; 0.285470;,
    0.113926; 0.951415; 0.286019;,
    0.508652; 0.000000; 0.860958;,
    -0.084109; 0.954680; 0.285470;,
    -0.000549; 0.999329; 0.036592;,
    -0.014191; 0.998199; 0.057863;,
    0.113926; 0.951415; 0.286019;,
    0.113926; 0.951415; 0.286019;,
    -0.000214; 0.997742; 0.066988;,
    -0.006928; 0.695700; 0.718253;,
    0.527787; 0.591144; 0.609851;,
    -0.000214; 0.997742; 0.066988;,
    -0.114963; 0.952422; 0.282174;,
    -0.542955; 0.584033; 0.603351;,
    -0.006928; 0.695700; 0.718253;,
    0.508652; 0.000000; 0.860958;,
    0.113926; 0.951415; 0.286019;,
    0.527787; 0.591144; 0.609851;,
    0.654378; 0.000000; 0.756127;,
    -0.114963; 0.952422; 0.282174;,
    -0.518784; 0.000000; 0.854885;,
    -0.668935; 0.000000; 0.743309;,
    -0.542955; 0.584033; 0.603351;,
    -0.000214; -0.997742; 0.066988;,
    0.113926; -0.951415; 0.285989;,
    0.527787; -0.591144; 0.609851;,
    -0.006928; -0.695700; 0.718253;,
    -0.114963; -0.952422; 0.282174;,
    -0.000214; -0.997742; 0.066988;,
    -0.006928; -0.695700; 0.718253;,
    -0.542955; -0.584033; 0.603351;,
    0.113926; -0.951415; 0.285989;,
    0.508652; 0.000000; 0.860958;,
    0.654378; 0.000000; 0.756127;,
    0.527787; -0.591144; 0.609851;,
    -0.518784; 0.000000; 0.854885;,
    -0.114963; -0.952422; 0.282174;,
    -0.542955; -0.584033; 0.603351;,
    -0.668935; 0.000000; 0.743309;,
    -0.006928; -0.695700; 0.718253;,
    -0.009644; 0.000000; 0.999939;,
    -0.668935; 0.000000; 0.743309;,
    -0.542955; -0.584033; 0.603351;,
    0.527787; -0.591144; 0.609851;,
    0.654378; 0.000000; 0.756127;,
    -0.009644; 0.000000; 0.999939;,
    -0.006928; -0.695700; 0.718253;,
    -0.009644; 0.000000; 0.999939;,
    -0.006928; 0.695700; 0.718253;,
    -0.542955; 0.584033; 0.603351;,
    -0.668935; 0.000000; 0.743309;,
    0.654378; 0.000000; 0.756127;,
    0.527787; 0.591144; 0.609851;,
    -0.006928; 0.695700; 0.718253;,
    -0.009644; 0.000000; 0.999939;,
    -0.000214; 0.997742; -0.066988;,
    0.113987; 0.951720; -0.284951;,
    0.704611; 0.709525; -0.005371;,
    0.000000; 1.000000; 0.000000;,
    -0.114963; 0.952116; -0.283273;,
    -0.000214; 0.997742; -0.066988;,
    0.000000; 1.000000; 0.000000;,
    -0.709494; 0.704672; 0.005432;,
    0.113987; 0.951720; -0.284951;,
    0.510636; 0.000000; -0.859767;,
    0.999969; 0.000000; -0.007660;,
    0.704611; 0.709525; -0.005371;,
    -0.516831; 0.000000; -0.856075;,
    -0.114963; 0.952116; -0.283273;,
    -0.709494; 0.704672; 0.005432;,
    -0.999969; 0.000000; 0.007660;,
    0.113956; -0.951720; -0.284951;,
    -0.000214; -0.997742; -0.066988;,
    0.000000; -1.000000; 0.000000;,
    0.704611; -0.709525; -0.005371;,
    -0.000214; -0.997742; -0.066988;,
    -0.114963; -0.952116; -0.283273;,
    -0.709494; -0.704672; 0.005432;,
    0.000000; -1.000000; 0.000000;,
    0.510636; 0.000000; -0.859767;,
    0.113956; -0.951720; -0.284951;,
    0.704611; -0.709525; -0.005371;,
    0.999969; 0.000000; -0.007660;,
    -0.114963; -0.952116; -0.283273;,
    -0.516831; 0.000000; -0.856075;,
    -0.999969; 0.000000; 0.007660;,
    -0.709494; -0.704672; 0.005432;,
    -0.709494; -0.704672; 0.005432;,
    -0.999969; 0.000000; 0.007660;,
    -0.668935; 0.000000; -0.743309;,
    -0.542955; -0.584033; -0.603351;,
    0.999969; 0.000000; -0.007660;,
    0.704611; -0.709525; -0.005371;,
    0.527787; -0.591144; -0.609851;,
    0.654378; 0.000000; -0.756127;,
    0.000000; -1.000000; 0.000000;,
    -0.709494; -0.704672; 0.005432;,
    -0.542955; -0.584033; -0.603351;,
    -0.006928; -0.695700; -0.718253;,
    0.704611; -0.709525; -0.005371;,
    0.000000; -1.000000; 0.000000;,
    -0.006928; -0.695700; -0.718253;,
    0.527787; -0.591144; -0.609851;,
    -0.999969; 0.000000; 0.007660;,
    -0.709494; 0.704672; 0.005432;,
    -0.542955; 0.584033; -0.603351;,
    -0.668935; 0.000000; -0.743309;,
    0.704611; 0.709525; -0.005371;,
    0.999969; 0.000000; -0.007660;,
    0.654378; 0.000000; -0.756127;,
    0.527787; 0.591144; -0.609851;,
    -0.709494; 0.704672; 0.005432;,
    0.000000; 1.000000; 0.000000;,
    -0.006928; 0.695700; -0.718253;,
    -0.542955; 0.584033; -0.603351;,
    0.000000; 1.000000; 0.000000;,
    0.704611; 0.709525; -0.005371;,
    0.527787; 0.591144; -0.609851;,
    -0.006928; 0.695700; -0.718253;,
    -0.542955; -0.584033; -0.603351;,
    -0.668935; 0.000000; -0.743309;,
    -0.009644; 0.000000; -0.999939;,
    -0.006928; -0.695700; -0.718253;,
    -0.006928; -0.695700; -0.718253;,
    -0.009644; 0.000000; -0.999939;,
    0.654378; 0.000000; -0.756127;,
    0.527787; -0.591144; -0.609851;,
    -0.668935; 0.000000; -0.743309;,
    -0.542955; 0.584033; -0.603351;,
    -0.006928; 0.695700; -0.718253;,
    -0.009644; 0.000000; -0.999939;,
    -0.009644; 0.000000; -0.999939;,
    -0.006928; 0.695700; -0.718253;,
    0.527787; 0.591144; -0.609851;,
    0.654378; 0.000000; -0.756127;;
88;
4; 0, 3, 2, 1;,
4; 4, 7, 6, 5;,
4; 8, 11, 10, 9;,
4; 12, 15, 14, 13;,
4; 16, 19, 18, 17;,
4; 20, 23, 22, 21;,
4; 24, 27, 26, 25;,
4; 28, 31, 30, 29;,
4; 32, 35, 34, 33;,
4; 36, 39, 38, 37;,
4; 40, 43, 42, 41;,
4; 44, 47, 46, 45;,
4; 48, 51, 50, 49;,
4; 52, 55, 54, 53;,
4; 56, 59, 58, 57;,
4; 60, 63, 62, 61;,
4; 64, 67, 66, 65;,
4; 68, 71, 70, 69;,
4; 72, 75, 74, 73;,
4; 76, 79, 78, 77;,
4; 80, 83, 82, 81;,
4; 84, 87, 86, 85;,
4; 88, 91, 90, 89;,
4; 92, 95, 94, 93;,
4; 96, 99, 98, 97;,
4; 100, 103, 102, 101;,
4; 104, 107, 106, 105;,
4; 108, 111, 110, 109;,
4; 112, 115, 114, 113;,
4; 116, 119, 118, 117;,
4; 120, 123, 122, 121;,
4; 124, 127, 126, 125;,
4; 128, 131, 130, 129;,
4; 132, 135, 134, 133;,
4; 136, 139, 138, 137;,
4; 140, 143, 142, 141;,
4; 144, 147, 146, 145;,
4; 148, 151, 150, 149;,
4; 152, 155, 154, 153;,
4; 156, 159, 158, 157;,
4; 160, 163, 162, 161;,
4; 164, 167, 166, 165;,
4; 168, 171, 170, 169;,
4; 172, 175, 174, 173;,
4; 176, 179, 178, 177;,
4; 180, 183, 182, 181;,
4; 184, 187, 186, 185;,
4; 188, 191, 190, 189;,
4; 192, 195, 194, 193;,
4; 196, 199, 198, 197;,
4; 200, 203, 202, 201;,
4; 204, 207, 206, 205;,
4; 208, 211, 210, 209;,
4; 212, 215, 214, 213;,
4; 216, 219, 218, 217;,
4; 220, 223, 222, 221;,
4; 224, 227, 226, 225;,
4; 228, 231, 230, 229;,
4; 232, 235, 234, 233;,
4; 236, 239, 238, 237;,
4; 240, 243, 242, 241;,
4; 244, 247, 246, 245;,
4; 248, 251, 250, 249;,
4; 252, 255, 254, 253;,
4; 256, 259, 258, 257;,
4; 260, 263, 262, 261;,
4; 264, 267, 266, 265;,
4; 268, 271, 270, 269;,
4; 272, 275, 274, 273;,
4; 276, 279, 278, 277;,
4; 280, 283, 282, 281;,
4; 284, 287, 286, 285;,
4; 288, 291, 290, 289;,
4; 292, 295, 294, 293;,
4; 296, 299, 298, 297;,
4; 300, 303, 302, 301;,
4; 304, 307, 306, 305;,
4; 308, 311, 310, 309;,
4; 312, 315, 314, 313;,
4; 316, 319, 318, 317;,
4; 320, 323, 322, 321;,
4; 324, 327, 326, 325;,
4; 328, 331, 330, 329;,
4; 332, 335, 334, 333;,
4; 336, 339, 338, 337;,
4; 340, 343, 342, 341;,
4; 344, 347, 346, 345;,
4; 348, 351, 350, 349;;
}  //End of MeshNormals
MeshTextureCoords {
352;
0.421979;-0.503981;,
0.467982;-0.503981;,
0.467982;-0.594183;,
0.421979;-0.594183;,
0.467982;-0.503981;,
0.512181;-0.503981;,
0.512181;-0.594183;,
0.467982;-0.594183;,
0.421979;-0.413778;,
0.467982;-0.413778;,
0.467982;-0.503981;,
0.421979;-0.503981;,
0.467982;-0.413778;,
0.512181;-0.413778;,
0.512181;-0.503981;,
0.467982;-0.503981;,
0.421979;-0.503981;,
0.467982;-0.503981;,
0.467982;-0.413778;,
0.421979;-0.413778;,
0.467982;-0.503981;,
0.512181;-0.503981;,
0.512181;-0.413778;,
0.467982;-0.413778;,
0.421979;-0.594183;,
0.467982;-0.594183;,
0.467982;-0.503981;,
0.421979;-0.503981;,
0.467982;-0.594183;,
0.512181;-0.594183;,
0.512181;-0.503981;,
0.467982;-0.503981;,
0.512181;-0.503981;,
0.557283;-0.503981;,
0.557283;-0.594183;,
0.512181;-0.594183;,
0.557283;-0.503981;,
0.602384;-0.503981;,
0.602384;-0.594183;,
0.557283;-0.594183;,
0.512181;-0.413778;,
0.557283;-0.413778;,
0.557283;-0.503981;,
0.512181;-0.503981;,
0.557283;-0.413778;,
0.602384;-0.413778;,
0.602384;-0.503981;,
0.557283;-0.503981;,
0.512181;-0.503981;,
0.557283;-0.503981;,
0.557283;-0.413778;,
0.512181;-0.413778;,
0.602384;-0.413778;,
0.557283;-0.413778;,
0.557283;-0.503981;,
0.602384;-0.503981;,
0.512181;-0.594183;,
0.557283;-0.594183;,
0.557283;-0.503981;,
0.512181;-0.503981;,
0.557283;-0.594183;,
0.602384;-0.594183;,
0.602384;-0.503981;,
0.557283;-0.503981;,
0.602384;-0.503981;,
0.834655;-0.503981;,
0.737913;-0.769176;,
0.602384;-0.594183;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.602384;-0.413778;,
0.737913;-0.238785;,
0.834655;-0.503981;,
0.602384;-0.503981;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.189708;-0.503981;,
0.421979;-0.503981;,
0.421979;-0.594183;,
0.286450;-0.769176;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.286450;-0.238785;,
0.421979;-0.413778;,
0.421979;-0.503981;,
0.189708;-0.503981;,
0.467982;-0.413778;,
0.468321;-0.343623;,
0.512181;-0.350636;,
0.512181;-0.413778;,
0.421979;-0.413778;,
0.286450;-0.238785;,
0.468321;-0.343623;,
0.467982;-0.413778;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.467982;-0.594183;,
0.468321;-0.664338;,
0.286450;-0.769176;,
0.421979;-0.594183;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.512181;-0.594183;,
0.512181;-0.657325;,
0.468321;-0.664338;,
0.467982;-0.594183;,
0.557283;-0.413778;,
0.557621;-0.342744;,
0.737913;-0.238785;,
0.602384;-0.413778;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.512181;-0.413778;,
0.512181;-0.350636;,
0.557621;-0.342744;,
0.557283;-0.413778;,
0.557283;-0.594183;,
0.557621;-0.665218;,
0.512181;-0.657325;,
0.512181;-0.594183;,
0.602384;-0.594183;,
0.737913;-0.769176;,
0.557621;-0.665218;,
0.557283;-0.594183;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.834655;-0.503981;,
0.602384;-0.503981;,
0.602384;-0.594183;,
0.737913;-0.769176;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.737913;-0.238785;,
0.602384;-0.413778;,
0.602384;-0.503981;,
0.834655;-0.503981;,
0.421979;-0.503981;,
0.189708;-0.503981;,
0.286450;-0.769176;,
0.421979;-0.594183;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.421979;-0.413778;,
0.286450;-0.238785;,
0.189708;-0.503981;,
0.421979;-0.503981;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.468321;-0.343623;,
0.467982;-0.413778;,
0.512181;-0.413778;,
0.512181;-0.350636;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.286450;-0.238785;,
0.421979;-0.413778;,
0.467982;-0.413778;,
0.468321;-0.343623;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.468321;-0.664338;,
0.467982;-0.594183;,
0.421979;-0.594183;,
0.286450;-0.769176;,
0.512181;-0.657325;,
0.512181;-0.594183;,
0.467982;-0.594183;,
0.468321;-0.664338;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.602384;-0.413778;,
0.737913;-0.238785;,
0.557621;-0.342744;,
0.557283;-0.413778;,
0.512181;-0.350636;,
0.512181;-0.413778;,
0.557283;-0.413778;,
0.557621;-0.342744;,
0.557621;-0.665218;,
0.557283;-0.594183;,
0.512181;-0.594183;,
0.512181;-0.657325;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.790307;-0.457106;,
0.737913;-0.769176;,
0.602384;-0.594183;,
0.557283;-0.594183;,
0.557621;-0.665218;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
1.072850;-0.291331;,
1.074703;-0.160741;,
1.124137;-0.159082;,
1.122283;-0.291331;,
1.074221;-0.912610;,
1.072108;-0.791331;,
1.121542;-0.791331;,
1.123655;-0.911160;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
1.074703;-0.421921;,
1.072850;-0.291331;,
1.122283;-0.291331;,
1.124137;-0.423580;,
1.072108;-0.791331;,
1.074221;-0.670052;,
1.123655;-0.671502;,
1.121542;-0.791331;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.512773;-0.511378;,
0.008586;-0.994973;,
0.012242;-1.036655;,
0.002834;-0.061707;,
-0.000822;-0.020026;,
0.977485;-1.022032;,
0.973828;-0.985471;,
0.964421;-0.010521;,
0.968078;-0.047084;,
0.482674;-1.070779;,
0.008586;-0.994973;,
-0.000822;-0.020026;,
0.473269;-0.095833;,
0.973828;-0.985471;,
0.482674;-1.070779;,
0.473269;-0.095833;,
0.964421;-0.010521;,
0.012242;-1.036655;,
0.008586;-0.994973;,
-0.000822;-0.020026;,
0.002834;-0.061707;,
0.973828;-0.985471;,
0.977485;-1.022032;,
0.968078;-0.047084;,
0.964421;-0.010521;,
0.008586;-0.994973;,
0.482674;-1.070779;,
0.473269;-0.095833;,
-0.000822;-0.020026;,
0.482674;-1.070779;,
0.973828;-0.985471;,
0.964421;-0.010521;,
0.473269;-0.095833;,
-0.000822;-0.020026;,
0.002834;-0.061707;,
0.473269;-0.095833;,
0.473269;-0.095833;,
0.473269;-0.095833;,
0.473269;-0.095833;,
0.968078;-0.047084;,
0.964421;-0.010521;,
0.002834;-0.061707;,
-0.000822;-0.020026;,
0.473269;-0.095833;,
0.473269;-0.095833;,
0.473269;-0.095833;,
0.473269;-0.095833;,
0.964421;-0.010521;,
0.968078;-0.047084;;
}  //End of MeshTextureCoords
  }  // End of the Mesh Cube 
  }  // SI End of the Object Cube 
}  // End of the Root Frame