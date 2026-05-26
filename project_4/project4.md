# project4

Project4:ContributetoOpenCV
TheprojectwillcountastheequivalentofTWOprojectsinthefinalgrading.
ArtificialIntelligencetoolscangeneratecodequickly,buttheycannotreplacerealengineering
ability.Inrealsoftwaredevelopment,youstillneedtoreadlargecodebases,debugproblems,test
carefully,collaboratewithothers,andwritereliablecode.
Forthisproject,insteadofwritinganotherisolatedhomeworkprogram,youwillcontributetoareal
open-sourceproject:OpenCV.
OpenCVisoneofthemostwidelyusedcomputervisionlibrariesintheworld.Itiswrittenmainlyin
C++,usedinacademiaandindustry,andmaintainedbydevelopersworldwide.
YourgoalistostudytheOpenCVcontributionworkflow,modifyitssourcecode,andsubmita
meaningfulPullRequest(PR)afterinstructorreview.
Resources:
OfficialguideofOpenCV:https://github.com/opencv/opencv/wiki/How_to_contribute
Youcanfindsomeissuestobefixedat:https://github.com/opencv/opencv/issues
Ifyouhavenoideaonthecontribution,youcanalsochooseRISC-VoptimizationforOpenCV
functions.ThedescriptionaboutRISC-Voptimizationcanbefoundat
https://github.com/opencv/opencv/wiki/GSoC_2026#idea-risc-v-optimizations
Requirements
1.CreateaGitHubaccountifyoudonothaveone.
2.ReadtheOpenCVcontributionguide.
3.ForktheOpenCVrepositorytoyourownGitHubaccount.
4.Cloneyourforklocally.
5.BuildOpenCVsuccessfullyonyourcomputer.
6.Createanewbranchforyourwork.
7.Completeonecontributiontask(LevelA/B/C).
8.Submityourworktotheinstructor.
Step-by-StepStarterGuide(ForBeginners)
IfyouhaveneverusedGitHubbefore,followthesesteps.

Step1:ForkOpenCV
Openwebsiteinyourbrowser:
https://github.com/opencv/opencv
ClickFork.
Thiscreates:
yourname/opencv
Step2:CloneYourFork
gitclonehttps://github.com/yourname/opencv.git
cdopencv
Step3:AddOfficialRepository
git remote add upstream https://github.com/opencv/opencv.git
Check:
git remote -v
Step4:CreateYourOwnBranch
git checkout -b project4_yourname
Step5:BuildOpenCV
UseCMake+VisualStudio/LinuxMakefile/macOSXcode.
Step6:ModifyFiles
Changeonlyasmallnumberoffiles.

Step7:Commit
git add .
git commit -m"My test to improve error message in foo()"
Step8:Push
git push origin project4_yourname
ContributionLevels
Chooseonelevel.
LevelA(BeginnerFriendly)
SuitableforstudentswithweakC++backgroundornoGitHubexperience.
Yourgoalis:SuccessfullymodifyOpenCVandlearntheworkflow.
RecommendedTasks
A1.FixTyposinDocs/Comments
Examples:
grammarmistakes
spellingmistakes
unclearcomments
Searchfiles:
doc/
modules/*/*.cpp
modules/*/*.hpp
A2.ImproveErrorMessages
Example:
Old:

CV_Error(Error::StsBadArg,"Empty image");
Better:
CV_Error(Error::StsBadArg,
"Input image is empty. Please check imread() path.");
Searchkeyword:
CV_Error(
A3.AddaSmallSampleProgram
Example:
samples/cpp/
Writedemofor:
blur()
resize()
threshold()
canny()
A4.AddOneSimpleTest
Search:
modules/*/test/
Example:
Testifresize()workson1x1image.
MinimumRequirementforLevelA
1.BuildOpenCVsuccessfully.
2.Modifyatleastonefile.

3.Commitsuccessfully.
4.Reportclearlywhatyouchanged.
LevelB(Intermediate)
SuitableforstudentswhounderstandC/C++basics.
Yourgoalis:
Solveasmallrealengineeringproblem.
RecommendedTasks
Youcandosomethingsimilarbutnotexactlythesameasthefollowingexamples.
B1.ImproveParameterChecking
Examples:
negativewidth/height
nullpointer
invalidkernelsize
Search:
CV_Assert(
B2.FixSmallBugs
Examples:
cornercasecrash
wrongboundarybehavior
integeroverflowrisk
warningincompiler
TrysearchingGitHubIssuesathttps://github.com/opencv/opencv/issueswithkeywords:
bug small fix
B3.RefactorRepeatedCode
Findrepeatedcodeblocksandconverttoafunction.

B4.SmallPerformanceImprovement
Examples:
avoidrepeatedallocation
movevariableoutsideloop
reducecopyoperation
MinimumRequirementforLevelB
1.BuildOpenCV.
2.Modifycodelogic.
3.Testbefore/after.
4.Explainwhyyourfixisuseful.
LevelC(Advanced)
Suitableforstrongstudents.
Yourgoalis:Makeausefulrealcontribution.
RecommendedTasks
C1.AddNewUtilityFunction
C2.SIMD/ParallelOptimization
Use:
UniversalIntrinsicsdesignedbyOpenCV
SSE/AVX
ARMNEON
OpenMP/TBB/etc
C3.SolveRealOpenCVIssue
Findissuefromhttps://github.com/opencv/opencv/issues.
Fixitandprovidetest.
C4.AnyOtherUsefulContribution
MinimumRequirementforLevelC
1.Usefultechnicalcontribution.
2.Clearbenchmark/testing.

3.Professionalcodequality.
4.ReadyforPRreview.
ReportRequirements
Include:
1.Whichlevelyouselected.
2.Whatfilesyoumodified.
3.Before/Aftercomparison.
4.Howyoutestedit.
5.Difficultiesyoumet.
6.AItoolsused.
7.Whatyoulearned.
8.Don'tforgettoincludeyourGitHubrepositorylink.
Rules
1.Deadline:Pleasesubmityourprojectreportbeforeitsdeadline.Thedeadlineis23:59,May
31.Afterthedeadline(even1second),0score!Nodeadlineextensionforanystudents,even
forthosetheyenrollinthecourselate.
2.Format:Pleasesubmitthereportas:*.pdf.Onlyonefileisneededtosubmit.PDFformatcan
makethelayoutofthereporttobeconsistentonanycomputer.
