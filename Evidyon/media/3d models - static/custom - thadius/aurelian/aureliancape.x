xof 0303txt 0032
template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}

template VertexDuplicationIndices {
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}

template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array FLOAT weights[nWeights];
 Matrix4x4 matrixOffset;
}

template AnimTicksPerSecond {
 <9e415a43-7ba6-4a73-8743-b73d47e88476>
 DWORD AnimTicksPerSecond;
}


AnimTicksPerSecond {
 24;
}

Mesh {
 308;
 -0.400500;0.130000;-0.870000;,
 -0.720500;-0.077000;-0.870000;,
 -0.959300;0.087000;-0.347600;,
 -0.600000;0.294000;-0.347600;,
 -0.000500;0.330000;-0.870000;,
 0.000000;0.494000;-0.347600;,
 0.399500;0.130000;-0.870000;,
 0.600000;0.294000;-0.347600;,
 0.710000;-0.059000;-0.870000;,
 0.960700;0.087000;-0.347600;,
 -1.156000;0.226400;0.822500;,
 -0.700000;0.458100;0.822500;,
 0.000000;0.658100;0.822500;,
 0.700000;0.458100;0.822500;,
 1.169300;0.248100;0.822500;,
 -1.776300;0.202200;2.330000;,
 -0.901200;0.830000;2.330000;,
 -0.000300;1.030000;2.330000;,
 0.900600;0.830000;2.330000;,
 1.764200;0.179000;2.330000;,
 -0.720500;-0.107000;-0.870000;,
 -0.959300;0.057000;-0.347600;,
 -1.142200;0.201900;0.822500;,
 -1.760800;0.177600;2.330000;,
 -0.507300;-0.224800;-1.316200;,
 -0.353600;-0.039600;-1.416900;,
 -0.373300;-0.101900;-1.333100;,
 -0.453600;-0.166600;-1.297900;,
 -0.002500;0.081000;-1.482600;,
 0.002700;0.028500;-1.454000;,
 0.341900;-0.028800;-1.422800;,
 0.401000;-0.116500;-1.357900;,
 0.513400;-0.201600;-1.328800;,
 0.468300;-0.171200;-1.312000;,
 1.746800;0.156000;2.330000;,
 1.153900;0.225100;0.822500;,
 0.960700;0.057000;-0.347600;,
 0.710000;-0.089000;-0.870000;,
 -0.901200;0.730000;2.330000;,
 -0.000300;0.930000;2.330000;,
 0.900600;0.730000;2.330000;,
 -0.400500;0.030000;-0.870000;,
 -0.600000;0.194000;-0.347600;,
 -0.000500;0.230000;-0.870000;,
 0.000000;0.394000;-0.347600;,
 0.399500;0.030000;-0.870000;,
 0.600000;0.194000;-0.347600;,
 -0.700000;0.358100;0.822500;,
 0.000000;0.558100;0.822500;,
 0.700000;0.358100;0.822500;,
 -0.619300;-0.167300;-1.181000;,
 -0.401900;0.035000;-1.291000;,
 -0.002300;0.183500;-1.371800;,
 0.391900;0.043700;-1.295700;,
 0.620900;-0.144000;-1.193700;,
 -0.499900;-0.146000;-1.242500;,
 0.535000;-0.140100;-1.229100;,
 -0.397800;-0.074600;-1.281300;,
 0.001900;0.115600;-1.334900;,
 0.399700;-0.075500;-1.280200;,
 -0.002400;-0.364600;-1.178500;,
 -0.002400;-0.346100;-1.324700;,
 -0.002400;-0.386300;-1.324700;,
 -0.002400;-0.407100;-1.178500;,
 0.203700;-0.260100;-1.229300;,
 0.271300;-0.313600;-1.203800;,
 0.204300;-0.271300;-1.341300;,
 0.238000;-0.318700;-1.318400;,
 -0.246100;-0.316500;-1.210500;,
 -0.195100;-0.259600;-1.229900;,
 -0.182800;-0.264000;-1.328900;,
 -0.223000;-0.316400;-1.311300;,
 -0.362400;-0.202500;-1.286900;,
 -0.356800;-0.157800;-1.256000;,
 -0.356800;-0.157800;-1.317800;,
 -0.424200;-0.202500;-1.201800;,
 -0.451900;-0.157800;-1.186900;,
 -0.393100;-0.157800;-1.206000;,
 -0.524200;-0.202500;-1.234300;,
 -0.547000;-0.157800;-1.256000;,
 -0.510600;-0.157800;-1.206000;,
 -0.524200;-0.202500;-1.339400;,
 -0.510600;-0.157800;-1.367800;,
 -0.547000;-0.157800;-1.317800;,
 -0.424200;-0.202500;-1.371900;,
 -0.393100;-0.157800;-1.367800;,
 -0.451900;-0.157800;-1.386900;,
 -0.383000;-0.210300;-1.236900;,
 -0.478100;-0.210300;-1.206000;,
 -0.536900;-0.210300;-1.286900;,
 -0.478100;-0.210300;-1.367800;,
 -0.383000;-0.210300;-1.336900;,
 -0.435600;-0.242800;-1.236900;,
 -0.399300;-0.242800;-1.286900;,
 -0.451900;-0.257800;-1.286900;,
 -0.494400;-0.242800;-1.256000;,
 -0.494400;-0.242800;-1.317800;,
 -0.435600;-0.242800;-1.336900;,
 -0.400500;0.130000;-0.870000;,
 -0.400500;0.130000;-0.870000;,
 -0.720500;-0.077000;-0.870000;,
 -0.959300;0.087000;-0.347600;,
 -0.959300;0.087000;-0.347600;,
 -0.600000;0.294000;-0.347600;,
 -0.600000;0.294000;-0.347600;,
 -0.600000;0.294000;-0.347600;,
 -0.000500;0.330000;-0.870000;,
 -0.000500;0.330000;-0.870000;,
 0.000000;0.494000;-0.347600;,
 0.000000;0.494000;-0.347600;,
 0.000000;0.494000;-0.347600;,
 0.399500;0.130000;-0.870000;,
 0.399500;0.130000;-0.870000;,
 0.600000;0.294000;-0.347600;,
 0.600000;0.294000;-0.347600;,
 0.600000;0.294000;-0.347600;,
 0.710000;-0.059000;-0.870000;,
 0.710000;-0.059000;-0.870000;,
 0.960700;0.087000;-0.347600;,
 0.960700;0.087000;-0.347600;,
 -1.156000;0.226400;0.822500;,
 -1.156000;0.226400;0.822500;,
 -0.700000;0.458100;0.822500;,
 -0.700000;0.458100;0.822500;,
 -0.700000;0.458100;0.822500;,
 0.000000;0.658100;0.822500;,
 0.000000;0.658100;0.822500;,
 0.000000;0.658100;0.822500;,
 0.700000;0.458100;0.822500;,
 0.700000;0.458100;0.822500;,
 0.700000;0.458100;0.822500;,
 1.169300;0.248100;0.822500;,
 1.169300;0.248100;0.822500;,
 -1.776300;0.202200;2.330000;,
 -1.776300;0.202200;2.330000;,
 -0.901200;0.830000;2.330000;,
 -0.901200;0.830000;2.330000;,
 -0.901200;0.830000;2.330000;,
 -0.000300;1.030000;2.330000;,
 -0.000300;1.030000;2.330000;,
 -0.000300;1.030000;2.330000;,
 0.900600;0.830000;2.330000;,
 0.900600;0.830000;2.330000;,
 0.900600;0.830000;2.330000;,
 1.764200;0.179000;2.330000;,
 1.764200;0.179000;2.330000;,
 -0.720500;-0.107000;-0.870000;,
 -0.720500;-0.107000;-0.870000;,
 -0.720500;-0.107000;-0.870000;,
 -0.959300;0.057000;-0.347600;,
 -0.959300;0.057000;-0.347600;,
 -0.959300;0.057000;-0.347600;,
 -1.142200;0.201900;0.822500;,
 -1.142200;0.201900;0.822500;,
 -1.142200;0.201900;0.822500;,
 -1.760800;0.177600;2.330000;,
 -1.760800;0.177600;2.330000;,
 -0.507300;-0.224800;-1.316200;,
 -0.353600;-0.039600;-1.416900;,
 -0.353600;-0.039600;-1.416900;,
 -0.373300;-0.101900;-1.333100;,
 -0.373300;-0.101900;-1.333100;,
 -0.373300;-0.101900;-1.333100;,
 -0.373300;-0.101900;-1.333100;,
 -0.453600;-0.166600;-1.297900;,
 -0.453600;-0.166600;-1.297900;,
 -0.453600;-0.166600;-1.297900;,
 -0.002500;0.081000;-1.482600;,
 -0.002500;0.081000;-1.482600;,
 0.002700;0.028500;-1.454000;,
 0.002700;0.028500;-1.454000;,
 0.341900;-0.028800;-1.422800;,
 0.341900;-0.028800;-1.422800;,
 0.401000;-0.116500;-1.357900;,
 0.401000;-0.116500;-1.357900;,
 0.401000;-0.116500;-1.357900;,
 0.513400;-0.201600;-1.328800;,
 0.513400;-0.201600;-1.328800;,
 0.468300;-0.171200;-1.312000;,
 0.468300;-0.171200;-1.312000;,
 0.468300;-0.171200;-1.312000;,
 1.746800;0.156000;2.330000;,
 1.153900;0.225100;0.822500;,
 0.960700;0.057000;-0.347600;,
 0.710000;-0.089000;-0.870000;,
 -0.901200;0.730000;2.330000;,
 -0.901200;0.730000;2.330000;,
 -0.000300;0.930000;2.330000;,
 -0.000300;0.930000;2.330000;,
 0.900600;0.730000;2.330000;,
 0.900600;0.730000;2.330000;,
 -0.400500;0.030000;-0.870000;,
 -0.400500;0.030000;-0.870000;,
 -0.400500;0.030000;-0.870000;,
 -0.600000;0.194000;-0.347600;,
 -0.600000;0.194000;-0.347600;,
 -0.600000;0.194000;-0.347600;,
 -0.000500;0.230000;-0.870000;,
 -0.000500;0.230000;-0.870000;,
 -0.000500;0.230000;-0.870000;,
 0.000000;0.394000;-0.347600;,
 0.000000;0.394000;-0.347600;,
 0.000000;0.394000;-0.347600;,
 0.399500;0.030000;-0.870000;,
 0.399500;0.030000;-0.870000;,
 0.399500;0.030000;-0.870000;,
 0.600000;0.194000;-0.347600;,
 0.600000;0.194000;-0.347600;,
 0.600000;0.194000;-0.347600;,
 -0.700000;0.358100;0.822500;,
 -0.700000;0.358100;0.822500;,
 -0.700000;0.358100;0.822500;,
 0.000000;0.558100;0.822500;,
 0.000000;0.558100;0.822500;,
 0.000000;0.558100;0.822500;,
 0.700000;0.358100;0.822500;,
 0.700000;0.358100;0.822500;,
 0.700000;0.358100;0.822500;,
 -0.619300;-0.167300;-1.181000;,
 -0.401900;0.035000;-1.291000;,
 -0.401900;0.035000;-1.291000;,
 -0.401900;0.035000;-1.291000;,
 -0.002300;0.183500;-1.371800;,
 -0.002300;0.183500;-1.371800;,
 -0.002300;0.183500;-1.371800;,
 0.391900;0.043700;-1.295700;,
 0.391900;0.043700;-1.295700;,
 0.391900;0.043700;-1.295700;,
 0.620900;-0.144000;-1.193700;,
 0.620900;-0.144000;-1.193700;,
 0.620900;-0.144000;-1.193700;,
 -0.499900;-0.146000;-1.242500;,
 -0.499900;-0.146000;-1.242500;,
 -0.499900;-0.146000;-1.242500;,
 -0.499900;-0.146000;-1.242500;,
 0.535000;-0.140100;-1.229100;,
 0.535000;-0.140100;-1.229100;,
 0.535000;-0.140100;-1.229100;,
 -0.397800;-0.074600;-1.281300;,
 -0.397800;-0.074600;-1.281300;,
 -0.397800;-0.074600;-1.281300;,
 -0.397800;-0.074600;-1.281300;,
 0.001900;0.115600;-1.334900;,
 0.001900;0.115600;-1.334900;,
 0.001900;0.115600;-1.334900;,
 0.399700;-0.075500;-1.280200;,
 0.399700;-0.075500;-1.280200;,
 0.399700;-0.075500;-1.280200;,
 0.399700;-0.075500;-1.280200;,
 -0.002400;-0.364600;-1.178500;,
 -0.002400;-0.364600;-1.178500;,
 -0.002400;-0.346100;-1.324700;,
 -0.002400;-0.386300;-1.324700;,
 -0.002400;-0.407100;-1.178500;,
 -0.002400;-0.407100;-1.178500;,
 0.203700;-0.260100;-1.229300;,
 0.203700;-0.260100;-1.229300;,
 0.203700;-0.260100;-1.229300;,
 0.271300;-0.313600;-1.203800;,
 0.271300;-0.313600;-1.203800;,
 0.271300;-0.313600;-1.203800;,
 0.204300;-0.271300;-1.341300;,
 0.204300;-0.271300;-1.341300;,
 0.204300;-0.271300;-1.341300;,
 0.238000;-0.318700;-1.318400;,
 0.238000;-0.318700;-1.318400;,
 0.238000;-0.318700;-1.318400;,
 -0.246100;-0.316500;-1.210500;,
 -0.246100;-0.316500;-1.210500;,
 -0.246100;-0.316500;-1.210500;,
 -0.195100;-0.259600;-1.229900;,
 -0.195100;-0.259600;-1.229900;,
 -0.195100;-0.259600;-1.229900;,
 -0.182800;-0.264000;-1.328900;,
 -0.182800;-0.264000;-1.328900;,
 -0.182800;-0.264000;-1.328900;,
 -0.223000;-0.316400;-1.311300;,
 -0.223000;-0.316400;-1.311300;,
 -0.223000;-0.316400;-1.311300;,
 -0.362400;-0.202500;-1.286900;,
 -0.362400;-0.202500;-1.286900;,
 -0.356800;-0.157800;-1.256000;,
 -0.356800;-0.157800;-1.317800;,
 -0.424200;-0.202500;-1.201800;,
 -0.424200;-0.202500;-1.201800;,
 -0.451900;-0.157800;-1.186900;,
 -0.393100;-0.157800;-1.206000;,
 -0.524200;-0.202500;-1.234300;,
 -0.524200;-0.202500;-1.234300;,
 -0.547000;-0.157800;-1.256000;,
 -0.510600;-0.157800;-1.206000;,
 -0.524200;-0.202500;-1.339400;,
 -0.524200;-0.202500;-1.339400;,
 -0.510600;-0.157800;-1.367800;,
 -0.547000;-0.157800;-1.317800;,
 -0.424200;-0.202500;-1.371900;,
 -0.424200;-0.202500;-1.371900;,
 -0.393100;-0.157800;-1.367800;,
 -0.383000;-0.210300;-1.236900;,
 -0.478100;-0.210300;-1.206000;,
 -0.536900;-0.210300;-1.286900;,
 -0.478100;-0.210300;-1.367800;,
 -0.383000;-0.210300;-1.336900;,
 -0.435600;-0.242800;-1.236900;,
 -0.399300;-0.242800;-1.286900;,
 -0.494400;-0.242800;-1.256000;,
 -0.494400;-0.242800;-1.317800;,
 -0.435600;-0.242800;-1.336900;;
 186;
 3;99,105,102;,
 3;99,102,100;,
 3;107,110,104;,
 3;107,104,0;,
 3;112,115,109;,
 3;112,109,4;,
 3;117,119,114;,
 3;117,114,6;,
 3;103,124,121;,
 3;103,121,2;,
 3;108,127,123;,
 3;108,123,3;,
 3;113,130,126;,
 3;113,126,5;,
 3;118,132,129;,
 3;118,129,7;,
 3;122,137,134;,
 3;122,134,10;,
 3;125,140,136;,
 3;125,136,11;,
 3;128,143,139;,
 3;128,139,12;,
 3;131,145,142;,
 3;131,142,13;,
 3;101,151,148;,
 3;101,148,100;,
 3;120,154,150;,
 3;120,150,2;,
 3;133,156,153;,
 3;133,153,10;,
 3;157,166,163;,
 3;157,163,159;,
 3;25,162,169;,
 3;25,169,168;,
 3;28,170,174;,
 3;28,174,172;,
 3;30,175,180;,
 3;30,180,177;,
 3;131,182,181;,
 3;131,181,144;,
 3;118,183,35;,
 3;118,35,14;,
 3;117,37,36;,
 3;117,36,9;,
 3;135,185,155;,
 3;135,155,15;,
 3;138,187,186;,
 3;138,186,16;,
 3;141,189,188;,
 3;141,188,17;,
 3;19,34,190;,
 3;19,190,18;,
 3;149,196,193;,
 3;149,193,147;,
 3;195,202,199;,
 3;195,199,192;,
 3;201,208,205;,
 3;201,205,198;,
 3;207,183,184;,
 3;207,184,204;,
 3;152,211,194;,
 3;152,194,21;,
 3;210,214,200;,
 3;210,200,42;,
 3;213,217,206;,
 3;213,206,44;,
 3;216,182,36;,
 3;216,36,46;,
 3;23,185,209;,
 3;23,209,22;,
 3;38,187,212;,
 3;38,212,47;,
 3;39,189,215;,
 3;39,215,48;,
 3;40,34,35;,
 3;40,35,49;,
 3;157,158,221;,
 3;157,221,218;,
 3;50,220,98;,
 3;50,98,100;,
 3;25,167,224;,
 3;25,224,219;,
 3;51,223,106;,
 3;51,106,0;,
 3;28,171,227;,
 3;28,227,222;,
 3;52,226,111;,
 3;52,111,4;,
 3;230,225,30;,
 3;230,30,176;,
 3;53,229,116;,
 3;53,116,6;,
 3;1,146,234;,
 3;1,234,218;,
 3;50,233,165;,
 3;50,165,24;,
 3;32,179,236;,
 3;32,236,228;,
 3;54,237,184;,
 3;54,184,8;,
 3;20,191,241;,
 3;20,241,232;,
 3;41,197,244;,
 3;41,244,240;,
 3;239,243,169;,
 3;239,169,161;,
 3;43,203,248;,
 3;43,248,242;,
 3;58,247,174;,
 3;58,174,29;,
 3;45,37,236;,
 3;45,236,246;,
 3;250,254,62;,
 3;250,62,61;,
 3;245,235,260;,
 3;245,260,257;,
 3;256,259,63;,
 3;256,63,60;,
 3;173,59,255;,
 3;173,255,263;,
 3;262,64,249;,
 3;262,249,61;,
 3;56,178,266;,
 3;56,266,258;,
 3;65,265,62;,
 3;65,62,253;,
 3;33,31,261;,
 3;33,261,264;,
 3;67,66,251;,
 3;67,251,252;,
 3;231,238,272;,
 3;231,272,269;,
 3;268,271,249;,
 3;268,249,253;,
 3;57,160,275;,
 3;57,275,270;,
 3;69,274,251;,
 3;69,251,60;,
 3;164,55,267;,
 3;164,267,278;,
 3;277,68,63;,
 3;277,63,252;,
 3;26,27,276;,
 3;26,276,273;,
 3;70,71,62;,
 3;70,62,61;,
 3;280,282,281;,
 3;284,286,285;,
 3;288,290,289;,
 3;292,294,293;,
 3;296,86,297;,
 3;298,72,73;,
 3;73,77,298;,
 3;284,298,77;,
 3;299,283,76;,
 3;76,80,299;,
 3;288,299,80;,
 3;300,287,79;,
 3;79,83,300;,
 3;292,300,83;,
 3;301,291,82;,
 3;82,86,301;,
 3;296,301,86;,
 3;302,295,85;,
 3;85,74,302;,
 3;280,302,74;,
 3;303,87,283;,
 3;87,303,304;,
 3;94,304,303;,
 3;304,279,87;,
 3;305,88,287;,
 3;88,305,92;,
 3;94,92,305;,
 3;92,75,88;,
 3;306,89,291;,
 3;89,306,95;,
 3;94,95,306;,
 3;95,78,89;,
 3;307,90,295;,
 3;90,307,96;,
 3;94,96,307;,
 3;96,81,90;,
 3;93,91,72;,
 3;91,93,97;,
 3;94,97,93;,
 3;97,84,91;;

 MeshNormals {
  308;
  -0.465516;-0.823998;0.322989;,
  -0.879252;-0.235912;0.413838;,
  -0.840744;-0.417290;0.344989;,
  -0.398614;-0.874260;0.277086;,
  -0.001862;-0.951396;0.307966;,
  0.000000;-0.966103;0.258158;,
  0.452636;-0.834253;0.314870;,
  0.389427;-0.878439;0.276933;,
  0.881906;-0.234783;0.408801;,
  0.836226;-0.418723;0.354114;,
  -0.904968;-0.314353;0.286732;,
  -0.383593;-0.901042;0.202432;,
  0.000000;-0.979012;0.203805;,
  0.377522;-0.904038;0.200480;,
  0.910707;-0.294972;0.289143;,
  -0.927238;-0.232679;-0.293413;,
  -0.312060;-0.719401;-0.620549;,
  0.000000;-0.780675;-0.624937;,
  0.311353;-0.716887;-0.623804;,
  0.927041;-0.193184;-0.321364;,
  -0.033388;0.984003;-0.174997;,
  -0.508693;0.860854;-0.012757;,
  -0.359297;0.932775;0.028932;,
  -0.208844;0.815295;-0.540072;,
  0.341172;0.918227;0.201151;,
  -0.170998;0.169777;0.970534;,
  0.599162;0.616834;0.510412;,
  0.138220;0.951798;0.273817;,
  0.008698;-0.057925;0.998283;,
  0.017029;0.983874;0.178048;,
  0.213819;0.074528;0.974026;,
  -0.455905;0.600784;0.656665;,
  -0.056399;0.907793;0.415610;,
  0.011353;0.947773;0.318742;,
  0.178872;0.833962;-0.522027;,
  0.339038;0.940419;0.025789;,
  0.506732;0.862093;-0.004303;,
  0.158120;0.980338;-0.118049;,
  0.223432;0.572725;-0.788711;,
  0.000000;0.601747;-0.798687;,
  -0.224654;0.575718;-0.786180;,
  0.395346;0.863113;-0.314226;,
  0.332596;0.904643;-0.266461;,
  -0.000519;0.952134;-0.305679;,
  0.000000;0.966103;-0.258158;,
  -0.378991;0.873070;-0.306781;,
  -0.321489;0.909014;-0.265212;,
  0.334369;0.921772;-0.196300;,
  0.000000;0.979012;-0.203805;,
  -0.327406;0.924688;-0.194313;,
  -0.848706;0.526394;0.051058;,
  -0.552611;-0.701452;0.450096;,
  -0.006836;-0.887012;0.461695;,
  0.525141;-0.727726;0.441183;,
  0.897009;0.411210;0.162116;,
  0.130804;0.831614;-0.539730;,
  -0.005402;0.875352;-0.483455;,
  0.647124;0.430806;-0.628996;,
  0.001099;0.909376;-0.415974;,
  -0.678721;0.472624;-0.562107;,
  -0.308948;-0.717174;-0.624670;,
  -0.387473;-0.561860;0.730875;,
  -0.318348;0.687845;0.652321;,
  -0.436613;0.562018;-0.702499;,
  -0.374776;-0.744670;-0.552276;,
  0.342309;0.529638;-0.776085;,
  -0.315237;-0.429044;0.846491;,
  0.334215;0.710182;0.619631;,
  -0.386034;0.541924;-0.746523;,
  0.347771;-0.696428;-0.627729;,
  0.345076;-0.526662;0.776885;,
  -0.347673;0.658723;0.667238;,
  -0.894433;-0.447202;0.000000;,
  -0.924894;-0.222396;0.308400;,
  -0.924894;-0.222396;-0.308400;,
  -0.276383;-0.447199;0.850662;,
  0.007477;-0.222421;0.974922;,
  -0.579108;-0.222426;0.784321;,
  0.723616;-0.447202;0.525728;,
  0.929520;-0.222423;0.294143;,
  0.566984;-0.222423;0.793131;,
  0.723616;-0.447202;-0.525728;,
  0.566984;-0.222423;-0.793131;,
  0.929520;-0.222423;-0.294143;,
  -0.276383;-0.447199;-0.850662;,
  -0.579108;-0.222426;-0.784321;,
  0.007477;-0.222421;-0.974922;,
  -0.688188;-0.525732;0.500004;,
  0.262866;-0.525732;0.809016;,
  0.850657;-0.525721;0.000000;,
  0.262866;-0.525732;-0.809016;,
  -0.688188;-0.525732;-0.500004;,
  -0.162453;-0.850656;0.499993;,
  -0.525721;-0.850657;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.425315;-0.850660;0.309006;,
  0.425315;-0.850660;-0.309006;,
  -0.162453;-0.850656;-0.499993;,
  -0.465516;-0.823998;0.322989;,
  -0.465516;-0.823998;0.322989;,
  -0.879252;-0.235912;0.413838;,
  -0.840744;-0.417290;0.344989;,
  -0.840744;-0.417290;0.344989;,
  -0.398614;-0.874260;0.277086;,
  -0.398614;-0.874260;0.277086;,
  -0.398614;-0.874260;0.277086;,
  -0.001862;-0.951396;0.307966;,
  -0.001862;-0.951396;0.307966;,
  0.000000;-0.966103;0.258158;,
  0.000000;-0.966103;0.258158;,
  0.000000;-0.966103;0.258158;,
  0.452636;-0.834253;0.314870;,
  0.452636;-0.834253;0.314870;,
  0.389427;-0.878439;0.276933;,
  0.389427;-0.878439;0.276933;,
  0.389427;-0.878439;0.276933;,
  0.881906;-0.234783;0.408801;,
  0.881906;-0.234783;0.408801;,
  0.836226;-0.418723;0.354114;,
  0.836226;-0.418723;0.354114;,
  -0.904968;-0.314353;0.286732;,
  -0.904968;-0.314353;0.286732;,
  -0.383593;-0.901042;0.202432;,
  -0.383593;-0.901042;0.202432;,
  -0.383593;-0.901042;0.202432;,
  0.000000;-0.979012;0.203805;,
  0.000000;-0.979012;0.203805;,
  0.000000;-0.979012;0.203805;,
  0.377522;-0.904038;0.200480;,
  0.377522;-0.904038;0.200480;,
  0.377522;-0.904038;0.200480;,
  0.910707;-0.294972;0.289143;,
  0.910707;-0.294972;0.289143;,
  -0.927238;-0.232679;-0.293413;,
  -0.927238;-0.232679;-0.293413;,
  -0.312060;-0.719401;-0.620549;,
  -0.312060;-0.719401;-0.620549;,
  -0.312060;-0.719401;-0.620549;,
  0.000000;-0.780675;-0.624937;,
  0.000000;-0.780675;-0.624937;,
  0.000000;-0.780675;-0.624937;,
  0.311353;-0.716887;-0.623804;,
  0.311353;-0.716887;-0.623804;,
  0.311353;-0.716887;-0.623804;,
  0.927041;-0.193184;-0.321364;,
  0.927041;-0.193184;-0.321364;,
  -0.033388;0.984003;-0.174997;,
  -0.033388;0.984003;-0.174997;,
  -0.033388;0.984003;-0.174997;,
  -0.508693;0.860854;-0.012757;,
  -0.508693;0.860854;-0.012757;,
  -0.508693;0.860854;-0.012757;,
  -0.359297;0.932775;0.028932;,
  -0.359297;0.932775;0.028932;,
  -0.359297;0.932775;0.028932;,
  -0.208844;0.815295;-0.540072;,
  -0.208844;0.815295;-0.540072;,
  0.341172;0.918227;0.201151;,
  -0.170998;0.169777;0.970534;,
  -0.170998;0.169777;0.970534;,
  0.599162;0.616834;0.510412;,
  0.599162;0.616834;0.510412;,
  0.599162;0.616834;0.510412;,
  0.599162;0.616834;0.510412;,
  0.138220;0.951798;0.273817;,
  0.138220;0.951798;0.273817;,
  0.138220;0.951798;0.273817;,
  0.008698;-0.057925;0.998283;,
  0.008698;-0.057925;0.998283;,
  0.017029;0.983874;0.178048;,
  0.017029;0.983874;0.178048;,
  0.213819;0.074528;0.974026;,
  0.213819;0.074528;0.974026;,
  -0.455905;0.600784;0.656665;,
  -0.455905;0.600784;0.656665;,
  -0.455905;0.600784;0.656665;,
  -0.056399;0.907793;0.415610;,
  -0.056399;0.907793;0.415610;,
  0.011353;0.947773;0.318742;,
  0.011353;0.947773;0.318742;,
  0.011353;0.947773;0.318742;,
  0.178872;0.833962;-0.522027;,
  0.339038;0.940419;0.025789;,
  0.506732;0.862093;-0.004303;,
  0.158120;0.980338;-0.118049;,
  0.223432;0.572725;-0.788711;,
  0.223432;0.572725;-0.788711;,
  0.000000;0.601747;-0.798687;,
  0.000000;0.601747;-0.798687;,
  -0.224654;0.575718;-0.786180;,
  -0.224654;0.575718;-0.786180;,
  0.395346;0.863113;-0.314226;,
  0.395346;0.863113;-0.314226;,
  0.395346;0.863113;-0.314226;,
  0.332596;0.904643;-0.266461;,
  0.332596;0.904643;-0.266461;,
  0.332596;0.904643;-0.266461;,
  -0.000519;0.952134;-0.305679;,
  -0.000519;0.952134;-0.305679;,
  -0.000519;0.952134;-0.305679;,
  0.000000;0.966103;-0.258158;,
  0.000000;0.966103;-0.258158;,
  0.000000;0.966103;-0.258158;,
  -0.378991;0.873070;-0.306781;,
  -0.378991;0.873070;-0.306781;,
  -0.378991;0.873070;-0.306781;,
  -0.321489;0.909014;-0.265212;,
  -0.321489;0.909014;-0.265212;,
  -0.321489;0.909014;-0.265212;,
  0.334369;0.921772;-0.196300;,
  0.334369;0.921772;-0.196300;,
  0.334369;0.921772;-0.196300;,
  0.000000;0.979012;-0.203805;,
  0.000000;0.979012;-0.203805;,
  0.000000;0.979012;-0.203805;,
  -0.327406;0.924688;-0.194313;,
  -0.327406;0.924688;-0.194313;,
  -0.327406;0.924688;-0.194313;,
  -0.848706;0.526394;0.051058;,
  -0.552611;-0.701452;0.450096;,
  -0.552611;-0.701452;0.450096;,
  -0.552611;-0.701452;0.450096;,
  -0.006836;-0.887012;0.461695;,
  -0.006836;-0.887012;0.461695;,
  -0.006836;-0.887012;0.461695;,
  0.525141;-0.727726;0.441183;,
  0.525141;-0.727726;0.441183;,
  0.525141;-0.727726;0.441183;,
  0.897009;0.411210;0.162116;,
  0.897009;0.411210;0.162116;,
  0.897009;0.411210;0.162116;,
  0.130804;0.831614;-0.539730;,
  0.130804;0.831614;-0.539730;,
  0.130804;0.831614;-0.539730;,
  0.130804;0.831614;-0.539730;,
  -0.005402;0.875352;-0.483455;,
  -0.005402;0.875352;-0.483455;,
  -0.005402;0.875352;-0.483455;,
  0.647124;0.430806;-0.628996;,
  0.647124;0.430806;-0.628996;,
  0.647124;0.430806;-0.628996;,
  0.647124;0.430806;-0.628996;,
  0.001099;0.909376;-0.415974;,
  0.001099;0.909376;-0.415974;,
  0.001099;0.909376;-0.415974;,
  -0.678721;0.472624;-0.562107;,
  -0.678721;0.472624;-0.562107;,
  -0.678721;0.472624;-0.562107;,
  -0.678721;0.472624;-0.562107;,
  -0.308948;-0.717174;-0.624670;,
  -0.308948;-0.717174;-0.624670;,
  -0.387473;-0.561860;0.730875;,
  -0.318348;0.687845;0.652321;,
  -0.436613;0.562018;-0.702499;,
  -0.436613;0.562018;-0.702499;,
  -0.374776;-0.744670;-0.552276;,
  -0.374776;-0.744670;-0.552276;,
  -0.374776;-0.744670;-0.552276;,
  0.342309;0.529638;-0.776085;,
  0.342309;0.529638;-0.776085;,
  0.342309;0.529638;-0.776085;,
  -0.315237;-0.429044;0.846491;,
  -0.315237;-0.429044;0.846491;,
  -0.315237;-0.429044;0.846491;,
  0.334215;0.710182;0.619631;,
  0.334215;0.710182;0.619631;,
  0.334215;0.710182;0.619631;,
  -0.386034;0.541924;-0.746523;,
  -0.386034;0.541924;-0.746523;,
  -0.386034;0.541924;-0.746523;,
  0.347771;-0.696428;-0.627729;,
  0.347771;-0.696428;-0.627729;,
  0.347771;-0.696428;-0.627729;,
  0.345076;-0.526662;0.776885;,
  0.345076;-0.526662;0.776885;,
  0.345076;-0.526662;0.776885;,
  -0.347673;0.658723;0.667238;,
  -0.347673;0.658723;0.667238;,
  -0.347673;0.658723;0.667238;,
  -0.894433;-0.447202;0.000000;,
  -0.894433;-0.447202;0.000000;,
  -0.924894;-0.222396;0.308400;,
  -0.924894;-0.222396;-0.308400;,
  -0.276383;-0.447199;0.850662;,
  -0.276383;-0.447199;0.850662;,
  0.007477;-0.222421;0.974922;,
  -0.579108;-0.222426;0.784321;,
  0.723616;-0.447202;0.525728;,
  0.723616;-0.447202;0.525728;,
  0.929520;-0.222423;0.294143;,
  0.566984;-0.222423;0.793131;,
  0.723616;-0.447202;-0.525728;,
  0.723616;-0.447202;-0.525728;,
  0.566984;-0.222423;-0.793131;,
  0.929520;-0.222423;-0.294143;,
  -0.276383;-0.447199;-0.850662;,
  -0.276383;-0.447199;-0.850662;,
  -0.579108;-0.222426;-0.784321;,
  -0.688188;-0.525732;0.500004;,
  0.262866;-0.525732;0.809016;,
  0.850657;-0.525721;0.000000;,
  0.262866;-0.525732;-0.809016;,
  -0.688188;-0.525732;-0.500004;,
  -0.162453;-0.850656;0.499993;,
  -0.525721;-0.850657;0.000000;,
  0.425315;-0.850660;0.309006;,
  0.425315;-0.850660;-0.309006;,
  -0.162453;-0.850656;-0.499993;;
  186;
  3;99,105,102;,
  3;99,102,100;,
  3;107,110,104;,
  3;107,104,0;,
  3;112,115,109;,
  3;112,109,4;,
  3;117,119,114;,
  3;117,114,6;,
  3;103,124,121;,
  3;103,121,2;,
  3;108,127,123;,
  3;108,123,3;,
  3;113,130,126;,
  3;113,126,5;,
  3;118,132,129;,
  3;118,129,7;,
  3;122,137,134;,
  3;122,134,10;,
  3;125,140,136;,
  3;125,136,11;,
  3;128,143,139;,
  3;128,139,12;,
  3;131,145,142;,
  3;131,142,13;,
  3;101,151,148;,
  3;101,148,100;,
  3;120,154,150;,
  3;120,150,2;,
  3;133,156,153;,
  3;133,153,10;,
  3;157,166,163;,
  3;157,163,159;,
  3;25,162,169;,
  3;25,169,168;,
  3;28,170,174;,
  3;28,174,172;,
  3;30,175,180;,
  3;30,180,177;,
  3;131,182,181;,
  3;131,181,144;,
  3;118,183,35;,
  3;118,35,14;,
  3;117,37,36;,
  3;117,36,9;,
  3;135,185,155;,
  3;135,155,15;,
  3;138,187,186;,
  3;138,186,16;,
  3;141,189,188;,
  3;141,188,17;,
  3;19,34,190;,
  3;19,190,18;,
  3;149,196,193;,
  3;149,193,147;,
  3;195,202,199;,
  3;195,199,192;,
  3;201,208,205;,
  3;201,205,198;,
  3;207,183,184;,
  3;207,184,204;,
  3;152,211,194;,
  3;152,194,21;,
  3;210,214,200;,
  3;210,200,42;,
  3;213,217,206;,
  3;213,206,44;,
  3;216,182,36;,
  3;216,36,46;,
  3;23,185,209;,
  3;23,209,22;,
  3;38,187,212;,
  3;38,212,47;,
  3;39,189,215;,
  3;39,215,48;,
  3;40,34,35;,
  3;40,35,49;,
  3;157,158,221;,
  3;157,221,218;,
  3;50,220,98;,
  3;50,98,100;,
  3;25,167,224;,
  3;25,224,219;,
  3;51,223,106;,
  3;51,106,0;,
  3;28,171,227;,
  3;28,227,222;,
  3;52,226,111;,
  3;52,111,4;,
  3;230,225,30;,
  3;230,30,176;,
  3;53,229,116;,
  3;53,116,6;,
  3;1,146,234;,
  3;1,234,218;,
  3;50,233,165;,
  3;50,165,24;,
  3;32,179,236;,
  3;32,236,228;,
  3;54,237,184;,
  3;54,184,8;,
  3;20,191,241;,
  3;20,241,232;,
  3;41,197,244;,
  3;41,244,240;,
  3;239,243,169;,
  3;239,169,161;,
  3;43,203,248;,
  3;43,248,242;,
  3;58,247,174;,
  3;58,174,29;,
  3;45,37,236;,
  3;45,236,246;,
  3;250,254,62;,
  3;250,62,61;,
  3;245,235,260;,
  3;245,260,257;,
  3;256,259,63;,
  3;256,63,60;,
  3;173,59,255;,
  3;173,255,263;,
  3;262,64,249;,
  3;262,249,61;,
  3;56,178,266;,
  3;56,266,258;,
  3;65,265,62;,
  3;65,62,253;,
  3;33,31,261;,
  3;33,261,264;,
  3;67,66,251;,
  3;67,251,252;,
  3;231,238,272;,
  3;231,272,269;,
  3;268,271,249;,
  3;268,249,253;,
  3;57,160,275;,
  3;57,275,270;,
  3;69,274,251;,
  3;69,251,60;,
  3;164,55,267;,
  3;164,267,278;,
  3;277,68,63;,
  3;277,63,252;,
  3;26,27,276;,
  3;26,276,273;,
  3;70,71,62;,
  3;70,62,61;,
  3;280,282,281;,
  3;284,286,285;,
  3;288,290,289;,
  3;292,294,293;,
  3;296,86,297;,
  3;298,72,73;,
  3;73,77,298;,
  3;284,298,77;,
  3;299,283,76;,
  3;76,80,299;,
  3;288,299,80;,
  3;300,287,79;,
  3;79,83,300;,
  3;292,300,83;,
  3;301,291,82;,
  3;82,86,301;,
  3;296,301,86;,
  3;302,295,85;,
  3;85,74,302;,
  3;280,302,74;,
  3;303,87,283;,
  3;87,303,304;,
  3;94,304,303;,
  3;304,279,87;,
  3;305,88,287;,
  3;88,305,92;,
  3;94,92,305;,
  3;92,75,88;,
  3;306,89,291;,
  3;89,306,95;,
  3;94,95,306;,
  3;95,78,89;,
  3;307,90,295;,
  3;90,307,96;,
  3;94,96,307;,
  3;96,81,90;,
  3;93,91,72;,
  3;91,93,97;,
  3;94,97,93;,
  3;97,84,91;;
 }

 MeshTextureCoords {
  308;
  0.000000;0.000000;,
  0.000000;-1.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;-1.000000;,
  0.000000;-1.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;-1.000000;,
  0.000000;0.000000;,
  0.000000;-1.000000;,
  -0.050000;-0.850000;,
  1.050000;-0.850000;,
  0.000000;-1.000000;,
  0.000000;0.000000;,
  0.000000;-1.000000;,
  1.050000;-0.850000;,
  0.000000;-1.000000;,
  -0.050000;-0.850000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  0.000000;-1.000000;,
  0.000000;-1.000000;,
  0.000000;-1.000000;,
  0.000000;-1.000000;,
  0.000000;0.000000;,
  0.000000;-1.000000;,
  0.000000;0.000000;,
  0.000000;-1.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;-1.000000;,
  0.000000;-1.000000;,
  0.000000;-1.000000;,
  0.000000;-1.000000;,
  0.000000;-1.000000;,
  1.050000;-0.850000;,
  -0.050000;-0.850000;,
  -0.050000;-0.850000;,
  0.000000;-1.000000;,
  1.050000;-0.850000;,
  -0.050000;-0.150000;,
  -0.050000;-0.150000;,
  1.050000;-0.150000;,
  1.050000;-0.150000;,
  1.050000;-0.850000;,
  -0.050000;-0.850000;,
  1.050000;-0.850000;,
  -0.050000;-0.850000;,
  1.050000;-0.850000;,
  -0.050000;-0.850000;,
  -0.050000;-0.850000;,
  1.050000;-0.850000;,
  0.000000;0.000000;,
  0.554256;-0.054256;,
  0.945744;-0.445744;,
  1.000000;0.000000;,
  0.500000;0.000000;,
  1.142054;-0.642054;,
  1.000000;0.000000;,
  0.456105;0.043895;,
  1.087794;-0.587794;,
  1.000000;0.000000;,
  0.412206;0.087794;,
  1.043895;-0.543895;,
  1.000000;0.000000;,
  0.357946;0.142054;,
  1.000000;-0.500000;,
  0.412211;0.175578;,
  0.412209;0.175581;,
  0.412211;0.175578;,
  0.412209;0.175581;,
  0.412211;0.175578;,
  1.000000;-0.412213;,
  0.412215;-0.412215;,
  1.000000;-1.000000;,
  1.000000;-0.412213;,
  1.000000;-0.412213;,
  1.000000;-0.412213;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  0.000000;0.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;-1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  0.000000;-1.000000;,
  1.000000;-1.000000;,
  0.000000;0.000000;,
  1.050000;-0.850000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  -0.050000;-0.850000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;-1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;-1.000000;,
  0.000000;0.000000;,
  -0.050000;-0.850000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;-1.000000;,
  0.000000;0.000000;,
  1.050000;-0.850000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  1.000000;-1.000000;,
  1.000000;-1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  1.000000;-1.000000;,
  1.000000;-1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;-1.000000;,
  1.000000;-1.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  -0.050000;-0.850000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  1.050000;-0.850000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  1.050000;-0.850000;,
  0.000000;-1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  -0.050000;-0.850000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  1.000000;0.000000;,
  1.050000;-0.150000;,
  -0.050000;-0.850000;,
  1.050000;-0.150000;,
  -0.050000;-0.150000;,
  -0.050000;-0.150000;,
  1.050000;-0.850000;,
  1.050000;-0.150000;,
  -0.050000;-0.850000;,
  -0.050000;-0.150000;,
  -0.050000;-0.150000;,
  1.050000;-0.850000;,
  1.050000;-0.150000;,
  1.050000;-0.150000;,
  -0.050000;-0.850000;,
  -0.050000;-0.150000;,
  -0.050000;-0.150000;,
  1.050000;-0.850000;,
  1.050000;-0.150000;,
  1.050000;-0.150000;,
  -0.050000;-0.850000;,
  -0.050000;-0.150000;,
  -0.050000;-0.150000;,
  1.050000;-0.850000;,
  1.050000;-0.150000;,
  -0.050000;-0.150000;,
  1.050000;-0.850000;,
  1.050000;-0.150000;,
  1.050000;-0.150000;,
  -0.050000;-0.850000;,
  -0.050000;-0.150000;,
  1.000000;0.000000;,
  1.000000;-1.000000;,
  0.456105;-0.500000;,
  1.043895;-0.500000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  0.500000;-0.500000;,
  1.087794;-0.500000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  0.554256;-0.500000;,
  1.142054;-0.500000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  0.357946;-0.500000;,
  0.945744;-0.500000;,
  0.000000;0.000000;,
  1.000000;-1.000000;,
  0.412206;-0.500000;,
  0.554256;-0.642045;,
  0.500000;-0.587791;,
  0.456106;-0.543894;,
  0.412209;-0.500000;,
  0.357955;-0.445744;,
  0.412213;-0.412213;,
  1.000000;-0.412215;,
  0.412213;-0.412213;,
  0.412213;-0.412213;,
  0.412213;-0.412213;;
 }

 VertexDuplicationIndices {
  308;
  98;
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  32,
  33,
  34,
  35,
  36,
  37,
  38,
  39,
  40,
  41,
  42,
  43,
  44,
  45,
  46,
  47,
  48,
  49,
  50,
  51,
  52,
  53,
  54,
  55,
  56,
  57,
  58,
  59,
  60,
  61,
  62,
  63,
  64,
  65,
  66,
  67,
  68,
  69,
  70,
  71,
  72,
  73,
  74,
  75,
  76,
  77,
  78,
  79,
  80,
  81,
  82,
  83,
  84,
  85,
  86,
  87,
  88,
  89,
  90,
  91,
  92,
  93,
  94,
  95,
  96,
  97,
  0,
  0,
  1,
  2,
  2,
  3,
  3,
  3,
  4,
  4,
  5,
  5,
  5,
  6,
  6,
  7,
  7,
  7,
  8,
  8,
  9,
  9,
  10,
  10,
  11,
  11,
  11,
  12,
  12,
  12,
  13,
  13,
  13,
  14,
  14,
  15,
  15,
  16,
  16,
  16,
  17,
  17,
  17,
  18,
  18,
  18,
  19,
  19,
  20,
  20,
  20,
  21,
  21,
  21,
  22,
  22,
  22,
  23,
  23,
  24,
  25,
  25,
  26,
  26,
  26,
  26,
  27,
  27,
  27,
  28,
  28,
  29,
  29,
  30,
  30,
  31,
  31,
  31,
  32,
  32,
  33,
  33,
  33,
  34,
  35,
  36,
  37,
  38,
  38,
  39,
  39,
  40,
  40,
  41,
  41,
  41,
  42,
  42,
  42,
  43,
  43,
  43,
  44,
  44,
  44,
  45,
  45,
  45,
  46,
  46,
  46,
  47,
  47,
  47,
  48,
  48,
  48,
  49,
  49,
  49,
  50,
  51,
  51,
  51,
  52,
  52,
  52,
  53,
  53,
  53,
  54,
  54,
  54,
  55,
  55,
  55,
  55,
  56,
  56,
  56,
  57,
  57,
  57,
  57,
  58,
  58,
  58,
  59,
  59,
  59,
  59,
  60,
  60,
  61,
  62,
  63,
  63,
  64,
  64,
  64,
  65,
  65,
  65,
  66,
  66,
  66,
  67,
  67,
  67,
  68,
  68,
  68,
  69,
  69,
  69,
  70,
  70,
  70,
  71,
  71,
  71,
  72,
  72,
  73,
  74,
  75,
  75,
  76,
  77,
  78,
  78,
  79,
  80,
  81,
  81,
  82,
  83,
  84,
  84,
  85,
  87,
  88,
  89,
  90,
  91,
  92,
  93,
  95,
  96,
  97;
 }

 MeshMaterialList {
  1;
  186;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;

  Material Mat3 {
   1.000000;1.000000;1.000000;1.000000;;
   1.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "blacktracery4.png";
   }
  }
 }
}