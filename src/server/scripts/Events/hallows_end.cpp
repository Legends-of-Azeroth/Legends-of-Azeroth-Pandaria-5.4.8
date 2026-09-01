/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "GameEventMgr.h"
#include "Vehicle.h"
#include "Random.h"

struct PostionEventoHallowend
{
    uint32 Area;
    uint8 Area_Count;
    bool AlreadyFired;
    Position SpawnPosition;
}

PostionEventoHallowends[] =
{
     87,    34,  false,  Position(-9456.598633f, 39.966614f,     62.195915f,  0.0f),
     87,    0,   false,  Position(-9456.162109f, 41.880970f,     62.011547f,  0.0f),
     87,    0,   false,  Position(-9455.980469f, 43.709557f,     62.056351f,  0.0f),
     87,    0,   false,  Position(-9461.942383f, 44.572384f,     62.474407f,  0.0f),
     87,    0,   false,  Position(-9461.984375f, 43.089458f,     62.598511f,  0.0f),
     87,    0,   false,  Position(-9462.203125f, 40.574947f,     62.672440f,  0.0f),
     87,    0,   false,  Position(-9475.374023f, 41.268730f,     64.304977f,  0.0f),
     87,    0,   false,  Position(-9471.819336f, 40.876884f,     64.311752f,  0.0f),
     87,    0,   false,  Position(-9466.619141f, 40.237621f,     64.321869f,  0.0f),
     87,    0,   false,  Position(-9462.197266f, 39.733025f,     64.328857f,  0.0f),
     87,    0,   false,  Position(-9457.063477f, 39.105618f,     64.323448f,  0.0f),
     87,    0,   false,  Position(-9452.693359f, 36.618862f,     69.778679f,  0.0f),
     87,    0,   false,  Position(-9456.606445f, 37.412331f,     69.696899f,  0.0f),
     87,    0,   false,  Position(-9462.572266f, 38.191925f,     69.859924f,  0.0f),
     87,    0,   false,  Position(-9476.938477f, 42.137085f,     71.250961f,  0.0f),
     87,    0,   false,  Position(-9479.271484f, 30.612669f,     70.269409f,  0.0f),
     87,    0,   false,  Position(-9478.808594f, 36.716347f,     64.324905f,  0.0f),
     87,    0,   false,  Position(-9479.466797f, 31.536119f,     64.346130f,  0.0f),
     87,    0,   false,  Position(-9479.929688f, 27.957096f,     64.348228f,  0.0f),
     87,    0,   false,  Position(-9480.361328f, 24.283165f,     64.418297f,  0.0f),
     87,    0,   false,  Position(-9452.841797f, 35.504967f,     64.328300f,  0.0f),
     87,    0,   false,  Position(-9453.877930f, 26.877312f,     64.328369f,  0.0f),
     87,    0,   false,  Position(-9453.395508f, 31.103748f,     64.328857f,  0.0f),
     87,    0,   false,  Position(-9465.262695f, 83.267693f,     66.926437f,  0.0f),
     87,    0,   false,  Position(-9466.190430f, 88.508972f,     66.678841f,  0.0f),
     87,    0,   false,  Position(-9461.586914f, 87.903183f,     68.835785f,  0.0f),
     87,    0,   false,  Position(-9461.333008f, 83.322281f,     68.954262f,  0.0f),
     87,    0,   false,  Position(-9457.314453f, 83.163277f,     68.416641f,  0.0f),
     87,    0,   false,  Position(-9457.511719f, 87.481514f,     68.503716f,  0.0f),
     87,    0,   false,  Position(-9454.168945f, 87.684364f,     66.910706f,  0.0f),
     87,    0,   false,  Position(-9453.876953f, 84.042046f,     66.780327f,  0.0f),
     87,    0,   false,  Position(-9452.500977f, 90.110939f,     66.963150f,  0.0f),
     87,    0,   false,  Position(-9453.014648f, 97.334198f,     67.853394f,  0.0f),
     87,    0,   false,  Position(-9466.933594f, 97.318192f,     67.665657f,  0.0f),
     131,   27,  false,  Position(-5578.172852f, -508.167419f,   404.096893f, 0.0f),
     131,   0,   false,  Position(-5577.978027f, -512.376343f,   404.096893f, 0.0f),
     131,   0,   false,  Position(-5578.129395f, -516.255676f,   404.097656f, 0.0f),
     131,   0,   false,  Position(-5578.432129f, -522.842163f,   404.096008f, 0.0f),
     131,   0,   false,  Position(-5579.825684f, -505.745697f,   404.096344f, 0.0f),
     131,   0,   false,  Position(-5584.710449f, -505.517365f,   404.096344f, 0.0f),
     131,   0,   false,  Position(-5585.525879f, -503.660217f,   413.178284f, 0.0f),
     131,   0,   false,  Position(-5582.077637f, -503.811554f,   413.373688f, 0.0f),
     131,   0,   false,  Position(-5577.631836f, -503.804688f,   413.265656f, 0.0f),
     131,   0,   false,  Position(-5576.027344f, -510.544647f,   413.286865f, 0.0f),
     131,   0,   false,  Position(-5576.213867f, -517.652893f,   413.300690f, 0.0f),
     131,   0,   false,  Position(-5592.137695f, -510.345764f,   413.121124f, 0.0f),
     131,   0,   false,  Position(-5596.859375f, -513.450806f,   413.181976f, 0.0f),
     131,   0,   false,  Position(-5602.766602f, -513.240051f,   413.275360f, 0.0f),
     131,   0,   false,  Position(-5609.043457f, -513.394104f,   413.381927f, 0.0f),
     131,   0,   false,  Position(-5588.285156f, -462.058838f,   414.503693f, 0.0f),
     131,   0,   false,  Position(-5588.110352f, -458.775116f,   414.375275f, 0.0f),
     131,   0,   false,  Position(-5588.123535f, -454.830292f,   414.161285f, 0.0f),
     131,   0,   false,  Position(-5583.522461f, -461.959167f,   414.408997f, 0.0f),
     131,   0,   false,  Position(-5577.516113f, -462.162292f,   414.318024f, 0.0f),
     131,   0,   false,  Position(-5572.210938f, -462.232544f,   414.228027f, 0.0f),
     131,   0,   false,  Position(-5570.547363f, -459.389496f,   414.081726f, 0.0f),
     131,   0,   false,  Position(-5570.349121f, -454.491058f,   413.896912f, 0.0f),
     131,   0,   false,  Position(-5573.891113f, -456.134583f,   403.206482f, 0.0f),
     131,   0,   false,  Position(-5584.926270f, -456.245667f,   403.206238f, 0.0f),
     131,   0,   false,  Position(-5582.571289f, -461.105408f,   402.603882f, 0.0f),
     131,   0,   false,  Position(-5576.388672f, -460.653290f,   402.603882f, 0.0f),
     3576,  28,  false,  Position(-4213.185547f, -12522.341797f, 49.787685f,  0.0f),
     3576,  0,   false,  Position(-4214.963867f, -12525.010742f, 50.883205f,  0.0f),
     3576,  0,   false,  Position(-4219.444336f, -12527.609375f, 50.422890f,  0.0f),
     3576,  0,   false,  Position(-4222.613281f, -12527.093750f, 54.527485f,  0.0f),
     3576,  0,   false,  Position(-4219.314941f, -12522.365234f, 55.642303f,  0.0f),
     3576,  0,   false,  Position(-4216.854492f, -12517.195313f, 55.820114f,  0.0f),
     3576,  0,   false,  Position(-4218.576660f, -12512.222656f, 53.843594f,  0.0f),
     3576,  0,   false,  Position(-4218.751465f, -12508.384766f, 48.981907f,  0.0f),
     3576,  0,   false,  Position(-4222.914063f, -12507.796875f, 49.336563f,  0.0f),
     3576,  0,   false,  Position(-4223.995117f, -12511.844727f, 53.319275f,  0.0f),
     3576,  0,   false,  Position(-4223.807129f, -12515.146484f, 56.371929f,  0.0f),
     3576,  0,   false,  Position(-4220.514160f, -12519.626953f, 57.187836f,  0.0f),
     3576,  0,   false,  Position(-4213.557129f, -12516.336914f, 54.507278f,  0.0f),
     3576,  0,   false,  Position(-4150.668457f, -12483.409180f, 51.244488f,  0.0f),
     3576,  0,   false,  Position(-4146.772461f, -12487.382813f, 51.631248f,  0.0f),
     3576,  0,   false,  Position(-4143.254883f, -12488.479492f, 48.431683f,  0.0f),
     3576,  0,   false,  Position(-4144.871094f, -12485.075195f, 55.429489f,  0.0f),
     3576,  0,   false,  Position(-4147.387207f, -12481.426758f, 57.274059f,  0.0f),
     3576,  0,   false,  Position(-4150.745605f, -12479.188477f, 48.477905f,  0.0f),
     3576,  0,   false,  Position(-4152.065430f, -12472.902344f, 50.862717f,  0.0f),
     3576,  0,   false,  Position(-4154.771973f, -12469.408203f, 52.474388f,  0.0f),
     3576,  0,   false,  Position(-4146.585938f, -12472.744141f, 54.687321f,  0.0f),
     3576,  0,   false,  Position(-4143.433105f, -12475.541992f, 58.953270f,  0.0f),
     3576,  0,   false,  Position(-4141.323242f, -12481.124023f, 59.377552f,  0.0f),
     3576,  0,   false,  Position(-4133.463379f, -12485.035156f, 53.983967f,  0.0f),
     3576,  0,   false,  Position(-4135.257813f, -12495.936523f, 51.745071f,  0.0f),
     3576,  0,   false,  Position(-4135.921387f, -12492.143555f, 48.833118f,  0.0f),
     3576,  0,   false,  Position(-4154.284180f, -12472.077148f, 51.846313f,  0.0f),
     362,   40,  false,  Position(336.829926f,   -4706.948730f,  16.889151f,  0.0f),
     362,   0,   false,  Position(339.642456f,   -4708.543945f,  18.006519f,  0.0f),
     362,   0,   false,  Position(342.224396f,   -4710.393066f,  16.818239f,  0.0f),
     362,   0,   false,  Position(344.960907f,   -4709.553711f,  16.890581f,  0.0f),
     362,   0,   false,  Position(347.107666f,   -4707.392090f,  16.872906f,  0.0f),
     362,   0,   false,  Position(349.523041f,   -4705.116699f,  16.835857f,  0.0f),
     362,   0,   false,  Position(351.904236f,   -4702.746094f,  16.814392f,  0.0f),
     362,   0,   false,  Position(354.097717f,   -4700.604004f,  16.788067f,  0.0f),
     362,   0,   false,  Position(356.199799f,   -4698.549805f,  16.764526f,  0.0f),
     362,   0,   false,  Position(358.804993f,   -4696.005371f,  16.734446f,  0.0f),
     362,   0,   false,  Position(360.704559f,   -4694.149902f,  16.711086f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(332.730408f,   -4712.049805f,  15.277740f,  0.0f),
     362,   0,   false,  Position(317.190308f,   -4772.762695f,  27.700096f,  0.0f),
     362,   0,   false,  Position(313.450867f,   -4773.237793f,  27.700096f,  0.0f),
     362,   0,   false,  Position(310.523895f,   -4773.609863f,  27.700096f,  0.0f),
     362,   0,   false,  Position(307.617767f,   -4773.979004f,  27.700096f,  0.0f),
     362,   0,   false,  Position(303.871460f,   -4774.482422f,  27.700096f,  0.0f),
     362,   0,   false,  Position(300.950226f,   -4774.166504f,  27.700096f,  0.0f),
     362,   0,   false,  Position(297.887360f,   -4774.443848f,  27.699116f,  0.0f),
     362,   0,   false,  Position(310.588226f,   -4777.041016f,  23.035875f,  0.0f),
     362,   0,   false,  Position(302.890533f,   -4777.734375f,  22.160141f,  0.0f),
     362,   0,   false,  Position(303.526398f,   -4783.147949f,  22.091099f,  0.0f),
     362,   0,   false,  Position(311.356537f,   -4782.477051f,  22.357243f,  0.0f),
     362,   0,   false,  Position(311.864349f,   -4786.790527f,  22.512373f,  0.0f),
     362,   0,   false,  Position(303.852966f,   -4787.567871f,  22.226116f,  0.0f),
     362,   0,   false,  Position(304.246216f,   -4792.124512f,  22.298147f,  0.0f),
     362,   0,   false,  Position(312.697449f,   -4791.288574f,  22.231749f,  0.0f),
     362,   0,   false,  Position(284.312897f,   -4721.923828f,  13.982411f,  0.0f),
     362,   0,   false,  Position(282.126221f,   -4720.415527f,  14.315186f,  0.0f),
     362,   0,   false,  Position(279.237915f,   -4718.960938f,  14.767025f,  0.0f),
     362,   0,   false,  Position(277.069824f,   -4717.780273f,  16.000343f,  0.0f),
     362,   0,   false,  Position(276.343201f,   -4715.700195f,  17.616024f,  0.0f),
     362,   0,   false,  Position(276.362915f,   -4713.054688f,  18.524426f,  0.0f),
     362,   0,   false,  Position(278.565430f,   -4710.165527f,  18.451063f,  0.0f),
     362,   0,   false,  Position(280.094330f,   -4707.666504f,  18.503857f,  0.0f),
     362,   0,   false,  Position(282.835510f,   -4707.406738f,  15.029071f,  0.0f),
     362,   0,   false,  Position(284.558929f,   -4709.244141f,  14.899632f,  0.0f),
     362,   0,   false,  Position(287.173279f,   -4711.354004f,  14.433277f,  0.0f),
     362,   0,   false,  Position(289.262390f,   -4713.011719f,  14.084586f,  0.0f),
     362,   0,   false,  Position(283.003418f,   -4701.726563f,  17.097305f,  0.0f),
     362,   0,   false,  Position(280.778656f,   -4701.861816f,  19.083368f,  0.0f),
     362,   0,   false,  Position(270.202362f,   -4720.975586f,  15.897130f,  0.0f),
     362,   0,   false,  Position(270.199463f,   -4716.922363f,  19.472425f,  0.0f),
     362,   0,   false,  Position(274.486603f,   -4719.081055f,  15.034330f,  0.0f),
     159,   15,  false,  Position(2249.25f,      239.413f,       43.5477f,    0.0f),
     159,   0,   false,  Position(2249.59f,      238.585f,       43.5459f,    0.0f),
     159,   0,   false,  Position(2248.17f,      242.901f,       43.6297f,    0.0f),
     159,   0,   false,  Position(2280.20f,      265.829f,       35.3957f,    0.0f),
     159,   0,   false,  Position(2275.53f,      270.947f,       39.9794f,    0.0f),
     159,   0,   false,  Position(2274.32f,      272.704f,       37.8483f,    0.0f),
     159,   0,   false,  Position(2271.19f,      279.613f,       35.3518f,    0.0f),
     159,   0,   false,  Position(2271.00f,      276.158f,       35.5703f,    0.0f),
     159,   0,   false,  Position(2259.57f,      268.819f,       40.6506f,    0.0f),
     159,   0,   false,  Position(2247.47f,      306.857f,       42.0482f,    0.0f),
     159,   0,   false,  Position(2242.94f,      302.805f,       42.12f,      0.0f),
     159,   0,   false,  Position(2238.71f,      242.756f,       46.3424f,    0.0f),
     159,   0,   false,  Position(2243.00f,      245.19f,        45.1917f,    0.0f),
     159,   0,   false,  Position(2255.32f,      227.241f,       43.5516f,    0.0f),
     159,   0,   false,  Position(2253.33f,      231.225f,       43.8722f,    0.0f),
     3665,  32,  false,  Position(9538.890625f,  -6864.580566f,  32.180435f,  0.0f),
     3665,  0,   false,  Position(9545.317383f,  -6864.680664f,  24.290354f,  0.0f),
     3665,  0,   false,  Position(9539.849609f,  -6863.418945f,  21.899117f,  0.0f),
     3665,  0,   false,  Position(9534.633789f,  -6860.602539f,  20.969927f,  0.0f),
     3665,  0,   false,  Position(9536.678711f,  -6862.365234f,  32.320389f,  0.0f),
     3665,  0,   false,  Position(9531.854492f,  -6862.107910f,  33.342640f,  0.0f),
     3665,  0,   false,  Position(9527.215820f,  -6861.431641f,  32.332523f,  0.0f),
     3665,  0,   false,  Position(9521.734375f,  -6861.298340f,  33.342361f,  0.0f),
     3665,  0,   false,  Position(9516.587891f,  -6860.852539f,  32.162258f,  0.0f),
     3665,  0,   false,  Position(9513.078125f,  -6864.157227f,  32.287502f,  0.0f),
     3665,  0,   false,  Position(9511.090820f,  -6862.834473f,  22.868782f,  0.0f),
     3665,  0,   false,  Position(9517.139648f,  -6861.322266f,  20.973946f,  0.0f),
     3665,  0,   false,  Position(9512.208008f,  -6855.778809f,  23.205914f,  0.0f),
     3665,  0,   false,  Position(9510.145508f,  -6851.798828f,  23.008202f,  0.0f),
     3665,  0,   false,  Position(9505.134766f,  -6848.893066f,  23.350080f,  0.0f),
     3665,  0,   false,  Position(9497.835938f,  -6848.266113f,  23.375212f,  0.0f),
     3665,  0,   false,  Position(9495.145508f,  -6849.114746f,  31.737856f,  0.0f),
     3665,  0,   false,  Position(9494.801758f,  -6845.622559f,  32.778965f,  0.0f),
     3665,  0,   false,  Position(9492.223633f,  -6842.419922f,  34.687244f,  0.0f),
     3665,  0,   false,  Position(9488.187500f,  -6838.931152f,  32.722904f,  0.0f),
     3665,  0,   false,  Position(9485.586914f,  -6836.182617f,  33.134155f,  0.0f),
     3665,  0,   false,  Position(9483.424805f,  -6833.842285f,  33.123482f,  0.0f),
     3665,  0,   false,  Position(9478.472656f,  -6833.402832f,  31.652319f,  0.0f),
     3665,  0,   false,  Position(9479.270508f,  -6833.205566f,  23.415434f,  0.0f),
     3665,  0,   false,  Position(9480.656250f,  -6829.862793f,  23.495510f,  0.0f),
     3665,  0,   false,  Position(9480.732422f,  -6824.777832f,  23.296629f,  0.0f),
     3665,  0,   false,  Position(9478.640625f,  -6820.736328f,  23.355219f,  0.0f),
     3665,  0,   false,  Position(9475.358398f,  -6818.134277f,  23.355270f,  0.0f),
     3665,  0,   false,  Position(9472.142578f,  -6816.050781f,  23.065981f,  0.0f),
     3665,  0,   false,  Position(9482.250977f,  -6833.041992f,  17.442104f,  0.0f),
     3665,  0,   false,  Position(9485.652344f,  -6835.646973f,  17.438766f,  0.0f),
     3665,  0,   false,  Position(9491.399414f,  -6842.145508f,  17.439112f,  0.0f),
     3665,  0,   false,  Position(9495.438477f,  -6846.233398f,  17.441633f,  0.0f),
     0,     0,   false,  Position(0.0f,          0.0f,           0.0f,        0.0f),
};

enum HallowendFire
{
    NPC_HEADLESS_HORSEMAN_FIRE_DND          = 23537,
    NPC_SHADE_OF_THE_HORSEMAN               = 23543,
    SPELL_HEADLES_HORSEMAN_QUEST_CREDIT     = 42242,
    SPELL_HEADLESS_HORSEMAN_START_FIRE      = 42132,
    SPELL_BUCKET_LANDS                      = 42339,
    SPELL_HEADLESS_HORSEMAN_FIRE_EXTINGUISH = 42348,
    SPELL_HEADLESS_HORSEMAN_LARGE_JACK      = 44231,
    // RangoFire
    SPELL_HEADLESS_HORSEMAN_BURNING         = 42971,
    SPELL_HEADLESS_HORSEMAN_FIRE            = 42074,
    SPELL_HEADLESS_HORSEMAN_VISUAL_FIRE     = 42075,
    GO_LARGE_JACK                           = 186887,
    GO_FIRE_EFFIGY                          = 186720,
};

#define ACTION_FAIL_EVENT       1
#define ACTION_START_EVENT      2
#define ACTION_PASS_EVENT       2
#define ACTION_SAY_1            3
#define ACTION_SAY_2            4
#define HALLOWEND_FIRE_ADD      1
#define HALLOWEND_FIRE_REMOVE   2

struct npc_hallowend : public ScriptedAI
{
    npc_hallowend(Creature* creature) : ScriptedAI(creature), Fires(creature)
    {
        Area = me->GetAreaId();
        AreaFire = 0;
        while (PostionEventoHallowends[AreaFire].Area && PostionEventoHallowends[AreaFire].Area != Area)
            AreaFire++;
        me->SetVisible(false);
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        for (uint8 j = 0; j < PostionEventoHallowends[AreaFire].Area_Count; j++)
        {
            if (Creature* summon = me->SummonCreature(NPC_HEADLESS_HORSEMAN_FIRE_DND, PostionEventoHallowends[AreaFire + j].SpawnPosition)->ToCreature())
            {
                Fires.Summon(summon);
                summon->AI()->SetData(0, AreaFire + j);
            }
        }
    }

    uint32 TimerBegin;
    uint32 TimerDuration;
    uint32 Area;
    uint32 CountPlayersEvent;
    uint8 AreaFire;
    SummonList Fires;
    bool EventProgress;
    bool SaidPhrase[2];

    void Reset() override
    {
        CountPlayersEvent = 0;
        EventProgress = false;
    }

    uint32 GetData(uint32 /*type*/) const override
    {
        return CountPlayersEvent;
    }

    uint32 PlayersCountRange(float dist) const
    {
        std::list<Player*> players;
        Trinity::AnyPlayerInObjectRangeCheck checker(me, dist);
        Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, players, checker);
        me->VisitNearbyWorldObject(dist, searcher);
        return players.size();
    }

    void EventComplete()
    {
        me->CastSpell(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 1, SPELL_HEADLESS_HORSEMAN_LARGE_JACK, true);
        DoCastAOE(SPELL_HEADLES_HORSEMAN_QUEST_CREDIT, true);
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case HALLOWEND_FIRE_REMOVE:
                PostionEventoHallowends[data].AlreadyFired = false;
                bool EventPassed = true;
                for (uint8 j = 0; j < PostionEventoHallowends[AreaFire].Area_Count; j++)
                    if (PostionEventoHallowends[AreaFire + j].AlreadyFired)
                        EventPassed = false;
                if (EventPassed)
                    EventEnd(EventPassed);
                break;
        }
    }

    void EventBegin()
    {
        CountPlayersEvent = PlayersCountRange(100.0f);
        if (!PlayersCountRange(100.0f))
            return;
        EventProgress = true;
        TimerDuration = 400 * IN_MILLISECONDS;
        for (uint8 j = 0; j < PostionEventoHallowends[AreaFire].Area_Count; j++) 
            PostionEventoHallowends[AreaFire + j].AlreadyFired = false;
        for (uint8 i = 0; i < 2; i++)
            SaidPhrase[i] = false;
        Fires.DoAction(NPC_HEADLESS_HORSEMAN_FIRE_DND, ACTION_START_EVENT);
        if (Creature* summon = me->SummonCreature(NPC_SHADE_OF_THE_HORSEMAN, 0, 0, 0))
            Fires.Summon(summon);
    }

    void EventEnd(bool eventPassed = false)
    {
        if (!eventPassed)
        {
            Fires.DoAction(NPC_HEADLESS_HORSEMAN_FIRE_DND, ACTION_FAIL_EVENT);
            Fires.DoAction(NPC_SHADE_OF_THE_HORSEMAN, ACTION_FAIL_EVENT);
        }
        else
        {
            EventComplete();
            Fires.DoAction(NPC_SHADE_OF_THE_HORSEMAN, ACTION_PASS_EVENT);
        }
        EventProgress = false;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!IsHolidayActive(HOLIDAY_HALLOWS_END))
            return;

        // The event begins each 1 / 4 of an hour. if during this process no one is around (10 seconds), the event began
        // Time Data works.
        // Example one hour 60 minutes we are starting every 15 minutes is divided in 4 (60 / 4) and this is therefore in seconds multiplied by 60 ((60 / 4) * 60)
        if (!EventProgress && (time(nullptr) % 900 < 5 || (900 - time(nullptr) % 900) < 5))
            EventBegin();

        if (EventProgress)
        {
            if (!SaidPhrase[0])
                if (TimerDuration <= 280*IN_MILLISECONDS)
                {
                    Fires.DoAction(NPC_SHADE_OF_THE_HORSEMAN, ACTION_SAY_1);
                    SaidPhrase[0] = true;
                } else TimerDuration -= diff;
            else 
                if (!SaidPhrase[1])
                    if (TimerDuration <= 130*IN_MILLISECONDS)
                    {
                        Fires.DoAction(NPC_SHADE_OF_THE_HORSEMAN, ACTION_SAY_2);
                        SaidPhrase[1] = true;
                    } else 
                        TimerDuration -= diff;
                else 
                    if (TimerDuration <= diff)
                    {
                        EventEnd();
                    } else TimerDuration -= diff;
        }
    }
};

enum WickermanGuardian
{
    SPELL_KNOCKDOWN                = 19128,
    SPELL_STRIKE                   = 18368,
    SPELL_WICKERMAN_GUARDIAN_EMBER = 25007,
};

struct npc_wickerman_guardian : public ScriptedAI
{
    npc_wickerman_guardian(Creature* creature) : ScriptedAI(creature) { }

    uint32 KnockdownTimer;
    uint32 StrikeTimer;

    void Reset() override
    {
        KnockdownTimer = 7000;
        StrikeTimer = 10000;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (KnockdownTimer <= diff)
        {
            DoCast(me->GetVictim(), SPELL_KNOCKDOWN);
            KnockdownTimer = 7000;
        } else KnockdownTimer -= diff;

        if (StrikeTimer <= diff)
        {
            DoCast(me->GetVictim(), SPELL_STRIKE);
            StrikeTimer = 10000;
        } else StrikeTimer -= diff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer) override
    {
        DoCast(killer, SPELL_WICKERMAN_GUARDIAN_EMBER, true);
    }
};

enum hallowen
{
    QUEST_INCOMING_GUMDROP  = 8358,
    EMOTE_TRAIN             = 264,
};

struct npc_kali_remik : public ScriptedAI
{
    npc_kali_remik(Creature* creature) : ScriptedAI(creature) { }

    void ReceiveEmote(Player* player, uint32 emote) override
    {
        if (!IsHolidayActive(HOLIDAY_HALLOWS_END))
            return;

        if (player->GetQuestStatus(QUEST_INCOMING_GUMDROP) == QUEST_STATUS_INCOMPLETE && emote == EMOTE_TRAIN)
            player->KilledMonsterCredit(me->GetEntry());
    }
};

// http://www.wowhead.com/npc=23537
// Headless Horseman - Fire (DND)
struct npc_headless_horseman_fire : public ScriptedAI
{
    npc_headless_horseman_fire(Creature* creature) : ScriptedAI(creature)
    {
        RangoFire[0] = SPELL_HEADLESS_HORSEMAN_BURNING;
        RangoFire[1] = SPELL_HEADLESS_HORSEMAN_FIRE;
        RangoFire[2] = SPELL_HEADLESS_HORSEMAN_VISUAL_FIRE;
        Immuned = true;
        Fire = false;
    }

    uint32 RangoFire[3];
    uint32 id = 0;
    uint32 RateFire;
    uint32 IncreaseFireTimer;
    bool Fire;
    bool Immuned;
    uint8 curRangoFire;

    void SetData(uint32 type, uint32 data) override
    {
        id = data;
    }

    uint32 GetData(uint32 /*type*/) const override
    {
        return id;
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_FAIL_EVENT:
                Immuned = true;
                break;
            case ACTION_START_EVENT:
                Immuned = false;
                Fire = false;
                RateFire = 0;
                curRangoFire = 0;
                me->RemoveAllAuras();
                break;
        }
    }

    uint32 PlayersCountRange(float dist) const
    {
        std::list<Player*> players;
        Trinity::AnyPlayerInObjectRangeCheck checker(me, dist);
        Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, players, checker);
        me->VisitNearbyWorldObject(dist, searcher);

        return players.size();
    }

    Creature* FireNodeNext()
    {
       std::list<Creature*> FireNodsList;
       GetCreatureListWithEntryInGrid(FireNodsList, me, NPC_HEADLESS_HORSEMAN_FIRE_DND, 15.0f);

       if (!FireNodsList.empty())
       {
           FireNodsList.sort(Trinity::ObjectDistanceOrderPred(me));

            for (auto* nodeFire : FireNodsList)
                if (!PostionEventoHallowends[nodeFire && nodeFire->AI()->GetData(0)].AlreadyFired)
                    return nodeFire;
       }
       return nullptr;
    }

    void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_BUCKET_LANDS && Fire && !Immuned)
        {
            if (PlayersCountRange(5.0f))
                return;

            me->CastSpell(me, SPELL_HEADLESS_HORSEMAN_FIRE_EXTINGUISH, true);
            me->RemoveAura(RangoFire[curRangoFire]);

            if (curRangoFire)
            {
                curRangoFire--;
                me->AddAura(RangoFire[curRangoFire], me);
            }
            else 
            {
                if (me->IsSummon())
                    if (Unit* eventKeeper =  me->ToTempSummon()->GetSummoner())
                        eventKeeper->ToCreature()->AI()->SetData(HALLOWEND_FIRE_REMOVE, id);
                Fire = false;
            }
            return;
        }
        if (spell->Id == SPELL_HEADLESS_HORSEMAN_START_FIRE)
        {
            me->AddAura(RangoFire[0], me);
            if (id >= 242)
            {
                TC_LOG_ERROR("scripts", "npc_headless_horseman_fire::SpellHit Shit happend! id: %u", id);
                return;
            }
            PostionEventoHallowends[id].AlreadyFired = true;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (me->HasAura(RangoFire[0]) || Fire)
        {
            if (me->HasAura(RangoFire[0]) && !Fire)
            {
                if (!RateFire)
                    if (Unit * Owner = me->ToTempSummon()->GetSummoner())
                        RateFire = Owner->ToCreature()->AI()->GetData(0);
                    else 
                        return;
                if (!RateFire)
                    return;
                Fire = true;
                IncreaseFireTimer = 60000 / RateFire;
            } else
                if (IncreaseFireTimer <= diff)
                {
                    if (curRangoFire < 2)
                    {
                        me->RemoveAura(RangoFire[curRangoFire]);
                        curRangoFire++;
                        me->AddAura(RangoFire[curRangoFire], me);
                    } else
                        if (Creature * nextFireNode = FireNodeNext())
                        {
                            nextFireNode->AddAura(RangoFire[0], nextFireNode);
                            PostionEventoHallowends[nextFireNode->AI()->GetData(0)].AlreadyFired = true;
                        }
                        IncreaseFireTimer = 60000 / RateFire;
                } else
                    IncreaseFireTimer -= diff;
        }
    }
};

// http://www.wowhead.com/npc=23543
// Shade of the Horseman

struct WaypointsShadeOfTheHorsemans
{
    uint32 area;
    bool CastPoint;
    Position waypoint;
}

WaypointsShadeOfTheHorsemans[] =
{
      87,    false,  Position(-9449.220703f, 99.542000f,     80.433723f,  0.0f),
      87,    false,  Position(-9487.250977f, 96.086182f,     76.224045f,  0.0f),
      87,    false,  Position(-9505.733398f, 85.762619f,     78.615189f,  0.0f),
      87,    false,  Position(-9515.541016f, 69.045929f,     78.518990f,  0.0f),
      87,    false,  Position(-9506.210938f, 49.220688f,     74.766258f,  0.0f),
      87,    false,  Position(-9493.707031f, 52.980251f,     73.841759f,  0.0f),
      87,    false,  Position(-9466.496094f, 59.418045f,     68.635506f,  0.0f),
      87,    true,   Position(-9451.565430f, 60.701469f,     69.817856f,  0.0f),
      87,    false,  Position(-9433.534180f, 56.966469f,     69.535995f,  0.0f),
      87,    false,  Position(-9426.815430f, 37.612507f,     69.721375f,  0.0f),
      87,    false,  Position(-9428.989258f, 20.016560f,     71.739769f,  0.0f),
      131,   false,  Position(-5549.735352f, -515.127075f,   417.435242f, 0.0f),
      131,   false,  Position(-5563.625977f, -494.025787f,   408.276031f, 0.0f),
      131,   false,  Position(-5576.229492f, -484.620575f,   407.339783f, 0.0f),
      131,   false,  Position(-5598.328613f, -481.467041f,   405.901550f, 0.0f),
      131,   false,  Position(-5618.585449f, -484.472595f,   405.008942f, 0.0f),
      131,   false,  Position(-5638.980469f, -484.686829f,   403.672089f, 0.0f),
      131,   false,  Position(-5653.769531f, -490.112793f,   404.059021f, 0.0f),
      131,   false,  Position(-5648.979492f, -507.761230f,   413.465271f, 0.0f),
      131,   false,  Position(-5633.484863f, -515.791504f,   415.236389f, 0.0f),
      131,   false,  Position(-5619.904297f, -504.694641f,   412.783020f, 0.0f),
      131,   true,   Position(-5599.156738f, -492.679260f,   408.886597f, 0.0f),
      131,   false,  Position(-5581.895020f, -482.131866f,   411.817047f, 0.0f),
      131,   false,  Position(-5551.732910f, -464.940369f,   418.337616f, 0.0f),
      3576,  false,  Position(-4181.044922f, -12545.703125f, 58.406044f,  0.0f),
      3576,  false,  Position(-4184.859863f, -12545.989258f, 66.531494f,  0.0f),
      3576,  false,  Position(-4162.623535f, -12534.581055f, 60.958271f,  0.0f),
      3576,  false,  Position(-4138.957031f, -12509.273438f, 58.346157f,  0.0f),
      3576,  false,  Position(-4145.405762f, -12485.465820f, 61.571053f,  0.0f),
      3576,  false,  Position(-4170.041016f, -12472.727539f, 62.661823f,  0.0f),
      3576,  false,  Position(-4177.905273f, -12487.194336f, 60.646717f,  0.0f),
      3576,  true,   Position(-4179.975098f, -12502.522461f, 55.405643f,  0.0f),
      3576,  false,  Position(-4187.343750f, -12508.174805f, 56.180138f,  0.0f),
      3576,  false,  Position(-4208.107422f, -12512.329102f, 59.904762f,  0.0f),
      362,   false,  Position(275.410431f,   -4664.126465f,  31.811525f,  0.0f),
      362,   false,  Position(289.195679f,   -4688.268066f,  28.616646f,  0.0f),
      362,   true,   Position(313.071777f,   -4734.405762f,  27.163692f,  0.0f),
      362,   false,  Position(336.558136f,   -4735.292480f,  25.536400f,  0.0f),
      362,   false,  Position(364.464661f,   -4734.240723f,  23.702780f,  0.0f),
      159,   false,  Position(2288.239014f,  370.882111f,    52.932304f,  0.0f),
      159,   false,  Position(2283.253174f,  357.807556f,    52.963966f,  0.0f),
      159,   false,  Position(2266.459473f,  319.605682f,    52.030453f,  0.0f),
      159,   true,   Position(2247.472656f,  284.688538f,    47.141811f,  0.0f),
      159,   false,  Position(2241.526123f,  277.526276f,    47.969597f,  0.0f),
      159,   false,  Position(2201.554688f,  251.503479f,    52.721680f,  0.0f),
      3665,  false,  Position(9553.414063f,  -6814.344727f,  48.652557f,  0.0f),
      3665,  false,  Position(9545.339844f,  -6787.827637f,  44.812103f,  0.0f),
      3665,  false,  Position(9524.579102f,  -6766.849609f,  41.753448f,  0.0f),
      3665,  false,  Position(9502.037109f,  -6770.908203f,  40.224274f,  0.0f),
      3665,  false,  Position(9486.791992f,  -6789.646973f,  38.460819f,  0.0f),
      3665,  true,   Position(9505.909180f,  -6822.906250f,  36.336525f,  0.0f),
      3665,  false,  Position(9530.319336f,  -6826.905273f,  33.300465f,  0.0f),
      3665,  false,  Position(9546.786133f,  -6818.094727f,  34.542740f,  0.0f),
      3665,  false,  Position(9550.537109f,  -6816.340332f,  34.288609f,  0.0f),
      0,     false,  Position(0.0f,          0.0f,           0.0f,        0.0f),
};

enum HorsemanTalk
{
    HORSEMAN_INTRO_1    = 0,
    HORSEMAN_EVENT_FAIL = 1,
    HORSEMAN_PASS_EVENT = 2,
    HORSEMAN_SAY_1      = 3,
    HORSEMAN_SAY_2      = 4,
    HORSEMAN_UNUSED     = 5, // NYI
};

struct npc_shade_of_the_horseman : public ScriptedAI
{
    npc_shade_of_the_horseman(Creature* creature) : ScriptedAI(creature)
    {
        PointFisrtArea = 0;
        area = me->GetAreaId();
        while (WaypointsShadeOfTheHorsemans[PointFisrtArea].area && WaypointsShadeOfTheHorsemans[PointFisrtArea].area != area)
            PointFisrtArea++;
        TimerEventUI = 400*IN_MILLISECONDS;
        wp_reached = true;
        FlyMode();
        MovementFinished = false;
    }

    uint32 TimerEventUI;
    uint32 CountFire;
    uint32 PointCur;
    uint32 area;
    uint32 PointFisrtArea;
    bool wp_reached;
    bool MovementFinished;

    void FlyMode()
    {
        me->SetVisible(false);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlying(true);
        me->Mount(25159);
        me->SetSpeed(MOVE_WALK, 5.0f, true);
        PointCur = 0;
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_FAIL_EVENT:
                Talk(HORSEMAN_EVENT_FAIL);
                me->DisappearAndDie();
                break;
            case ACTION_PASS_EVENT:
                Talk(HORSEMAN_PASS_EVENT);
                me->DisappearAndDie();
                break;
            case ACTION_SAY_1:
                Talk(HORSEMAN_SAY_1);
                break;
            case ACTION_SAY_2:
                Talk(HORSEMAN_SAY_2);
                break;
        }
    }

    void CreateFires()
    {
        std::list<Creature*> FireNodsList;
        FireNodsList.clear();
        if (Unit * Owner = me->ToTempSummon()->GetSummoner())
            CountFire = Owner->ToCreature()->AI()->GetData(0);
        GetCreatureListWithEntryInGrid(FireNodsList, me, NPC_HEADLESS_HORSEMAN_FIRE_DND, 150.0f);
        uint32 CountFireer = 0;
        if (!FireNodsList.empty())
        {
            while (CountFireer <= CountFire)
            {
                std::list<Creature*>::iterator itr = FireNodsList.begin();
                if (itr == FireNodsList.end())
                    break;
                std::advance(itr, urand(0, FireNodsList.size()-1));
                me->CastSpell((*itr),SPELL_HEADLESS_HORSEMAN_START_FIRE,true);
                CountFireer++;
                FireNodsList.erase(itr);
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type != POINT_MOTION_TYPE)
            return;
        wp_reached = true;
        if (id == 0)
            me->SetVisible(true);

        if (WaypointsShadeOfTheHorsemans[PointFisrtArea + PointCur].CastPoint) 
        {
            CreateFires();
            Talk(HORSEMAN_INTRO_1);
        }

        PointCur++;
        if (WaypointsShadeOfTheHorsemans[PointFisrtArea + PointCur].area != area) 
        {
            MovementFinished = true;
            me->SetVisible(false);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (wp_reached && !MovementFinished)
        {
            wp_reached = false;
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MovePoint(PointCur,WaypointsShadeOfTheHorsemans[PointFisrtArea + PointCur].waypoint);
        }
    }
};

std::vector<uint32> stinkBombsAwayQuest         = { 29054, 29374  };
std::vector<uint32> stinkBombsAwaySummonBroom   = { 96020, 100042 };
std::vector<uint32> stinkBombsAwayTeleport      = { 96005, 100028 };
std::vector<uint32> stinkBombsAwayBroom         = { 51710, 53761  };
std::vector<Position> stinkBombsAwayAwayPos     = { { -9062.031f, 447.286f, 109.411f, 0.0000f }, { 1803.98300f, 184.210f, 99.5180f, 0.00000f } };
std::vector<Position> stinkBombsAwayTeleportPos = { { 1717.2810f, 338.336f, 19.3060f, 5.2638f }, { -8989.2226f, 491.739f, 148.888f, 0.66138f } };

struct npc_start_stink_bombs_away : public ScriptedAI
{
    npc_start_stink_bombs_away(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (player->GetQuestStatus(stinkBombsAwayQuest[player->GetTeamId()]) != QUEST_STATUS_INCOMPLETE)
            return false;

        player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(me), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 /*action*/) override
    {
        player->CLOSE_GOSSIP_MENU();

        uint8 team = player->GetTeamId();

        player->CastSpell(player, stinkBombsAwaySummonBroom[team]);
        Creature* broom = GetClosestCreatureWithEntry(me, stinkBombsAwayBroom[team], 50.0f, true);
        Creature* starter = broom->SummonCreature(me->GetEntry(), me->GetPosition(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 420000ms);
        ObjectGuid guid = player->GetGUID();
        if (broom && starter && guid)
        {
            player->EnterVehicle(broom, 1);
            starter->EnterVehicle(broom, 0);

            player->GetMap()->LoadGrid(stinkBombsAwayTeleportPos[team].GetPositionX(), stinkBombsAwayTeleportPos[team].GetPositionY());
            starter->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            broom->SetSpeed(MOVE_WALK, 7.0f);
            broom->SetControlled(true, UNIT_STATE_ROOT);
            broom->m_Events.Schedule(1000, [broom, team]() { broom->GetMotionMaster()->MovePoint(0, stinkBombsAwayAwayPos[team]); });
            broom->m_Events.Schedule(10000, [broom, starter, guid, team]()
            {
                broom->CastSpell(broom, stinkBombsAwayTeleport[team], true);
                starter->UpdatePosition(stinkBombsAwayTeleportPos[team], true);
                if (Player* player = ObjectAccessor::GetPlayer(*broom, guid))
                    player->m_Events.Schedule(3000, [broom, team]()
                    {
                        broom->CastSpell(broom, stinkBombsAwayTeleport[team]);
                        broom->SetControlled(false, UNIT_STATE_ROOT);
                        broom->GetMotionMaster()->Clear();
                        broom->GetMotionMaster()->MovePath(broom->GetEntry() * 100, true);
                    });
            });
        }
        return true;
    }
};

/*######
## http://www.wowhead.com/object=180574
## Wickerman Ember
## go_wickerman_ember
######*/

#define GOSSIP_WICKERMAN_SPELL 6535

enum WickermanEmberGo
{
    SPELL_GRIM_VISAGE   = 24705,
};

class go_wickerman_ember : public GameObjectScript
{
    public:
        go_wickerman_ember() : GameObjectScript("go_wickerman_ember") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            player->ADD_GOSSIP_ITEM_DB(GOSSIP_WICKERMAN_SPELL, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* go, uint32 /*uiSender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            if (player->HasAura(SPELL_GRIM_VISAGE))
            {
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
                player->CLOSE_GOSSIP_MENU();
            }
            else if (action == GOSSIP_ACTION_INFO_DEF +1)
            {
                go->CastSpell(player, SPELL_GRIM_VISAGE);
                player->CLOSE_GOSSIP_MENU();
            }
            return true;
        }
};

// http://www.wowhead.com/item=20413
enum HalloweenWand
{
    SPELL_PIRATE_COSTUME_MALE           = 24708,
    SPELL_PIRATE_COSTUME_FEMALE         = 24709,
    SPELL_NINJA_COSTUME_MALE            = 24711,
    SPELL_NINJA_COSTUME_FEMALE          = 24710,
    SPELL_LEPER_GNOME_COSTUME_MALE      = 24712,
    SPELL_LEPER_GNOME_COSTUME_FEMALE    = 24713,
    SPELL_GHOST_COSTUME_MALE            = 24735,
    SPELL_GHOST_COSTUME_FEMALE          = 24736,

    SPELL_HALLOWEEN_WAND_PIRATE         = 24717,
    SPELL_HALLOWEEN_WAND_NINJA          = 24718,
    SPELL_HALLOWEEN_WAND_LEPER_GNOME    = 24719,
    SPELL_HALLOWEEN_WAND_RANDOM         = 24720,
    SPELL_HALLOWEEN_WAND_SKELETON       = 24724,
    SPELL_HALLOWEEN_WAND_WISP           = 24733,
    SPELL_HALLOWEEN_WAND_GHOST          = 24737,
    SPELL_HALLOWEEN_WAND_BAT            = 24741,
};

class spell_halloween_wand : public SpellScript
{
    PrepareSpellScript(spell_halloween_wand)

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_MALE))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_FEMALE))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_MALE))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_FEMALE))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_MALE))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_FEMALE))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_MALE))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_FEMALE))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_NINJA))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_LEPER_GNOME))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_RANDOM))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_SKELETON))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_WISP))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_GHOST))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_BAT))
            return false;
        return true;
    }

    void HandleScriptEffect()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitPlayer();

        if (!caster || !target)
            return;

        uint32 spellId = 0;
        uint8 gender = target->GetGender();

        switch (GetSpellInfo()->Id)
        {
            case SPELL_HALLOWEEN_WAND_LEPER_GNOME:
                spellId = gender ? SPELL_LEPER_GNOME_COSTUME_FEMALE : SPELL_LEPER_GNOME_COSTUME_MALE;
                break;
            case SPELL_HALLOWEEN_WAND_PIRATE:
                spellId = gender ? SPELL_PIRATE_COSTUME_FEMALE : SPELL_PIRATE_COSTUME_MALE;
                break;
            case SPELL_HALLOWEEN_WAND_GHOST:
                spellId = gender ? SPELL_GHOST_COSTUME_FEMALE : SPELL_GHOST_COSTUME_MALE;
                break;
            case SPELL_HALLOWEEN_WAND_NINJA:
                spellId = gender ? SPELL_NINJA_COSTUME_FEMALE : SPELL_NINJA_COSTUME_MALE;
                break;
            case SPELL_HALLOWEEN_WAND_RANDOM:
                spellId = RAND(SPELL_HALLOWEEN_WAND_PIRATE, SPELL_HALLOWEEN_WAND_NINJA, SPELL_HALLOWEEN_WAND_LEPER_GNOME, SPELL_HALLOWEEN_WAND_SKELETON, SPELL_HALLOWEEN_WAND_WISP, SPELL_HALLOWEEN_WAND_GHOST, SPELL_HALLOWEEN_WAND_BAT);
                break;
        }
        caster->CastSpell(target, spellId, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_halloween_wand::HandleScriptEffect);
    }
};

class spell_bucket_lands : public SpellScript
{
    PrepareSpellScript(spell_bucket_lands);

    bool Validate(SpellInfo const* /*SpellEntry*/) override
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_BUCKET_LANDS))
            return false;
        return true;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        if (!target || target->GetEntry() != NPC_HEADLESS_HORSEMAN_FIRE_DND)
            return;

        if (Player* player = GetCaster()->ToPlayer())
        {
            if (GameObject* effigy = player->FindNearestGameObject(GO_FIRE_EFFIGY, 70.0f))
            {
                player->KilledMonsterCredit(NPC_HEADLESS_HORSEMAN_FIRE_DND);
                target->ToCreature()->DespawnOrUnsummon();
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_bucket_lands::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class spell_clean_up_stink_bomb : public SpellScript
{
    PrepareSpellScript(spell_clean_up_stink_bomb);

    bool Validate(SpellInfo const* /*SpellEntry*/) override
    {
        if (!sSpellMgr->GetSpellInfo(101414))
            return false;
        return true;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        WorldLocation* loc = GetHitDest();
        if (!loc)
            return;

        std::list<GameObject*> list;
        caster->GetGameObjectListWithEntryInGrid(list, 208200, 50.0f);
        for (auto&& go : list)
        {
            if (go->isSpawned() && go->GetDistance(loc->GetPosition()) <= 5.0f)
            {
                go->ForcedDespawn();
                caster->CastSpell(caster, 97157, true);
                break;
            }
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_clean_up_stink_bomb::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class spell_stink_bombs_away_leave : public SpellScript
{
    PrepareSpellScript(spell_stink_bombs_away_leave);

    void HandleDummy()
    {
        if (Unit* broom = GetCaster())
        {
            broom->RemoveAura(stinkBombsAwayTeleport[0]);
            broom->RemoveAura(stinkBombsAwayTeleport[1]);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_stink_bombs_away_leave::HandleDummy);
    }
};

std::vector<uint32> stinkBombsAwayStarter     = { 52548, 53764 };
std::vector<uint32> stinkBombsAwayLeaveSpell  = { 96006 , 100027 };
std::vector<Position> stinkBombsAwayLeavePos  = { { -9049.577f, 430.529f, 131.260f, 3.44126f }, { 1839.848f, 300.567f, 120.972f, 4.502857f } };
std::vector<Position> stinkBombsAwayOutroPos  = { { -9109.716f, 413.499f, 98.4771f, 3.46264f }, { 1821.269f, 226.853f, 64.0249f, 4.206586f } };

class spell_stink_bombs_away_aura : public AuraScript
{
    PrepareAuraScript(spell_stink_bombs_away_aura);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* broom = GetOwner()->ToCreature())
        {
            Vehicle* broomVeh = broom->GetVehicleKit();
            if (!broomVeh)
                return;

            Unit* passenger = broomVeh->GetPassenger(1);
            if (!passenger)
                return;

            Player* player = passenger->ToPlayer();
            if (!player)
                return;

            uint8 team = player->GetTeamId();
            Creature* starter = GetClosestCreatureWithEntry(player, stinkBombsAwayStarter[team], 50.0f, true);
            ObjectGuid guid = player->GetGUID();

            if (broom && starter)
            {
                player->GetMap()->LoadGrid(stinkBombsAwayLeavePos[team].GetPositionX(), stinkBombsAwayLeavePos[team].GetPositionY());

                broom->GetMotionMaster()->Clear();
                broom->CastSpell(broom, stinkBombsAwayLeaveSpell[team]);
                broom->m_Events.Schedule(1000, [broom, team]() { broom->GetMotionMaster()->MovePoint(0, stinkBombsAwayOutroPos[team]); });
                broom->m_Events.Schedule(14000, [broom, starter, guid]()
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*broom, guid))
                        player->ExitVehicle();
                    broom->DespawnOrUnsummon();
                    starter->DespawnOrUnsummon();
                });
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_stink_bombs_away_aura::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_toss_stink_bomb_credit : public SpellScript
{
    PrepareSpellScript(spell_toss_stink_bomb_credit);

    void HandleDummy()
    {
        if (Unit* broom = GetCaster())
            if (Vehicle* broomVeh = broom->GetVehicleKit())
                if (Unit* passenger = broomVeh->GetPassenger(1))
                    if (Player* player = passenger->ToPlayer())
                        player->KilledMonsterCredit(GetSpellInfo()->Effects[EFFECT_0].MiscValue);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_toss_stink_bomb_credit::HandleDummy);
    }
};

enum HallowsendTrickyTreats
{
    SPELL_TRICKY_TREAT_STACKING_SPEED = 42919,
    SPELL_UPSET_TUMMY                 = 42966
};

class item_hallowsend_tricky_treat : public ItemScript
{
    public:
        item_hallowsend_tricky_treat() : ItemScript("item_hallowsend_tricky_treat") { }

       bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
       {
           if (player->HasAura(SPELL_UPSET_TUMMY))
           {
               player->SendEquipError(EQUIP_ERR_CLIENT_LOCKED_OUT, item);
               return true;
           }
           player->CastSpell(player, SPELL_TRICKY_TREAT_STACKING_SPEED, true);
           if (player->HasAura(SPELL_TRICKY_TREAT_STACKING_SPEED))
           {
               if (player->GetAura(SPELL_TRICKY_TREAT_STACKING_SPEED)->GetStackAmount() > 2 && roll_chance_i(20 * player->GetAura(SPELL_TRICKY_TREAT_STACKING_SPEED)->GetStackAmount()))
                   player->CastSpell(player, SPELL_UPSET_TUMMY,true);
           }
           return false;
       }
};

class at_shopping_around : public AreaTriggerScript
{
    public:
        at_shopping_around() : AreaTriggerScript("at_shopping_around") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (trigger->ID == 6922 && player->GetQuestStatus(29425) == QUEST_STATUS_INCOMPLETE)
            {
                Creature* delian = player->FindNearestCreature(54146, 50.0f);
                Creature* dran = player->FindNearestCreature(6986, 50.0f);
                ObjectGuid guid = player->GetGUID();
                if (delian && dran && !dran->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
                {
                    dran->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED); // just for prevent multiple triggering
                    delian->m_Events.Schedule(5000, [delian]() { delian->AI()->Talk(0); });
                    dran->m_Events.Schedule(10000, [dran]() { dran->AI()->Talk(0); });
                    delian->m_Events.Schedule(15000, [delian]() { delian->AI()->Talk(1); });
                    dran->m_Events.Schedule(20000, [dran]() { dran->AI()->Talk(1); });
                    delian->m_Events.Schedule(25000, [delian]() { delian->AI()->Talk(2); });
                    dran->m_Events.Schedule(30000, [dran]() { dran->AI()->Talk(2); });
                    delian->m_Events.Schedule(35000, [delian, guid]()
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*delian, guid))
                        {
                            player->KilledMonsterCredit(54022);
                            delian->AI()->Talk(3);
                        }
                    });
                    dran->m_Events.Schedule(35000, [dran]() { dran->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED); });
                }
            }

            if (trigger->ID == 6914 && player->GetQuestStatus(29399) == QUEST_STATUS_INCOMPLETE)
            {
                Creature* hudson = player->FindNearestCreature(54021, 50.0f);
                Creature* quincy = player->FindNearestCreature(53991, 50.0f);
                ObjectGuid guid = player->GetGUID();
                if (hudson && quincy && !quincy->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
                {
                    quincy->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED); // just for prevent multiple triggering
                    hudson->m_Events.Schedule(5000, [hudson]() { hudson->AI()->Talk(0); });
                    hudson->m_Events.Schedule(10000, [hudson]() { hudson->AI()->Talk(1); });
                    quincy->m_Events.Schedule(15000, [quincy]() { quincy->AI()->Talk(0); });
                    quincy->m_Events.Schedule(20000, [quincy]() { quincy->AI()->Talk(1); });
                    hudson->m_Events.Schedule(25000, [hudson]() { hudson->AI()->Talk(2); });
                    quincy->m_Events.Schedule(30000, [quincy]() { quincy->AI()->Talk(2); });
                    hudson->m_Events.Schedule(35000, [hudson, guid]()
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*hudson, guid))
                        {
                            player->KilledMonsterCredit(54022);
                            hudson->AI()->Talk(3, player);
                        }
                    });
                    quincy->m_Events.Schedule(35000, [quincy]() { quincy->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED); });
                }
            }

            return true;
        }
};

class at_the_collectors_agent : public AreaTriggerScript
{
    public:
        at_the_collectors_agent() : AreaTriggerScript("at_the_collectors_agent") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (player->GetQuestStatus(29427) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(29403) == QUEST_STATUS_INCOMPLETE)
            {
                if (Creature* sanath = player->FindNearestCreature(54072, 50.0f))
                {
                    ObjectGuid guid = player->GetGUID();
                    if (!sanath->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
                    {
                        sanath->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED); // just for prevent multiple triggering
                        sanath->m_Events.Schedule(5000, [sanath]() { sanath->AI()->Talk(0); });
                        sanath->m_Events.Schedule(15000, [sanath]() { sanath->AI()->Talk(1); });
                        sanath->m_Events.Schedule(20000, [sanath]()
                        {
                            sanath->SetStandState(UNIT_STAND_STATE_KNEEL);
                            sanath->HandleEmoteStateCommand(EMOTE_STATE_USE_STANDING);
                        });
                        sanath->m_Events.Schedule(25000, [sanath, guid, trigger]()
                        {
                            sanath->SetStandState(UNIT_STAND_STATE_STAND);
                            sanath->HandleEmoteStateCommand(EMOTE_ONESHOT_NONE);

                            Position pos = trigger->ID == 6924 ? Position({ 1579.189f, 61.371f, 62.5377f, 6.26843f }) : Position({ -8496.792f, 758.109f, 72.758f, 2.210720f });
                            if (Creature* uvoid = sanath->SummonCreature(54114, pos, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, Milliseconds(MINUTE * 2 * IN_MILLISECONDS)))
                                if (Player* player = ObjectAccessor::GetPlayer(*sanath, guid))
                                    uvoid->Attack(player, true);

                        });
                        sanath->m_Events.Schedule(30000, [sanath]()
                        {
                            sanath->AI()->Talk(2);
                            sanath->HandleEmoteStateCommand(EMOTE_STATE_COWER);
                        });
                        sanath->m_Events.Schedule(40000, [sanath]()
                        {
                            sanath->HandleEmoteStateCommand(EMOTE_ONESHOT_NONE);
                            sanath->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
                        });
                    }
                }
            }

            return true;
        }
};

void AddSC_event_hallows_end()
{
    new creature_script<npc_hallowend>("npc_hallowend");
    new creature_script<npc_wickerman_guardian>("npc_wickerman_guardian");
    new creature_script<npc_kali_remik>("npc_kali_remik");
    new creature_script<npc_headless_horseman_fire>("npc_headless_horseman_fire");
    new creature_script<npc_shade_of_the_horseman>("npc_shade_of_the_horseman");
    new creature_script<npc_start_stink_bombs_away>("npc_start_stink_bombs_away");
    new go_wickerman_ember;
    new spell_script<spell_halloween_wand>("spell_halloween_wand");
    new spell_script<spell_bucket_lands>("spell_bucket_lands");
    new spell_script<spell_clean_up_stink_bomb>("spell_clean_up_stink_bomb");
    new spell_script<spell_stink_bombs_away_leave>("spell_stink_bombs_away_leave");
    new aura_script<spell_stink_bombs_away_aura>("spell_stink_bombs_away_aura");
    new spell_script<spell_toss_stink_bomb_credit>("spell_toss_stink_bomb_credit");
    new item_hallowsend_tricky_treat();
    new at_shopping_around();
    new at_the_collectors_agent();
}
