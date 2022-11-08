# Damage Plasticity Model

The [!cite](lee1996theory) model accounts for the independent damage in tension and compression. It also accounts for degradation of the elastic modulus of the concrete as the loading goes beyond yielding in either tension or compression. The model uses the incremental theory of plasticity and decomposes the total strain, $\boldsymbol{\varepsilon}$, into elastic strain, $\boldsymbol{\varepsilon}^{e}$, and plastic strain, $\boldsymbol{\varepsilon}^{p}$, as follows
\begin{equation}
    \boldsymbol{\varepsilon} = \boldsymbol{\varepsilon}^{e} + \boldsymbol{\varepsilon}^{p} \label{eps_def}
\end{equation}
where bold symbol represents a vectoral or tensorial quantity. The relation between elastic strain and the stress, $\boldsymbol{\sigma}$, is given by
\begin{equation}
    \boldsymbol{\varepsilon}^{e} = \boldsymbol{\mathfrak{E}}^{-1}:\boldsymbol{\sigma} \label{eps_e_def}
\end{equation}
where $\boldsymbol{\mathfrak{E}}$ is the elasticity tensor. Using Eqs. \eqref{eps_def}-\eqref{eps_e_def}, the relation between $\boldsymbol{\sigma}$ and $\boldsymbol{\varepsilon}^{p}$ is expressed as
\begin{equation}
    \boldsymbol{\sigma} = \boldsymbol{\mathfrak{E}}:\left(\boldsymbol{\varepsilon} - \boldsymbol{\varepsilon}^{e}\right)
\end{equation}
Since the model considers the effect of damage in elastic stiffness, an effective stress,
$\boldsymbol{\sigma}^{e}$, is defined, where the stress for a given strain always corresponds to the
undamaged elastic stiffness of the material, $\boldsymbol{\mathfrak{E}}_{0}$ The relation between
$\boldsymbol{\sigma}^{e}$, $\boldsymbol{\varepsilon}$, and $\boldsymbol{\varepsilon}^{p}$ is given by
\begin{equation}
    \boldsymbol{\sigma}^e = \boldsymbol{\mathfrak{E}}_0:\left(\boldsymbol{\varepsilon} - \boldsymbol{\varepsilon}^{e}\right)
\end{equation}
To consider the degradation of reinforced-concrete structures, an isotropic damage was
considered in concrete material. Hence, the relation between $\boldsymbol{\sigma}^e$ and $\boldsymbol{\sigma}$ can be established by
the isotropic scalar degradation damage variable, D, as follows
\begin{equation}
    \boldsymbol{\sigma} = \left(1-D\right)\boldsymbol{\sigma}^e \label{sigma_def}
\end{equation}
\begin{equation}
    \boldsymbol{\sigma} = \left(1-D\right)\boldsymbol{\mathfrak{E}}_0:\left(\boldsymbol{\varepsilon} - \boldsymbol{\varepsilon}^{e}\right)\label{sigma_def2}
\end{equation}
The Damage Plasticity Model has various attributes to define the mechanical behavior of concrete
in tension and compression such as the yield function, plastic potential, strength of material
in tension and compression, and hardening and softening of the yield surface. These attributes
are discussed in detail in the following sections. A method for the implementation of the
Damage Plasticity Model and for the estimation of crack width are also presented in the upcoming
sections.

## Yield Function

The yield function, $\mathfrak{F}$ is a function of $\boldsymbol{\sigma}$, the strength of the material in uniaxial tension, $f_t$, and the strength of the material in uniaxial compression, $f_c$. It was used to describe the admissible stress space. For this implementation, the yield function in stress space is defined as follows
\begin{equation} \label{yf}
\begin{gathered}
    \mathfrak{F}\left(\boldsymbol{\sigma},f_t,f_c\right) = \frac{1}{1-\alpha} \\
    \left(\alpha I_1 + \sqrt{3J_2} + \beta\left(\boldsymbol{\kappa}\right)<{\hat{\boldsymbol{\sigma}}_{max}}>\right) - f_c\left(\boldsymbol{\kappa}\right)    
\end{gathered}
\end{equation}
where $I_1$ and $J_2$ is first invariant of stress and second invariant of the deviatoric
component of the stress, respectively, $<x> =\frac{x+|x|}{2}$ is the Macaulay bracket function, ${\hat{\boldsymbol{\sigma}}_{max}}$ is algebraically maximum principal
stress, $\alpha = \frac{f_{b0}-f_{c0}}{2 f_{b0}-f_{c0}}$ is a parameter that relates
uniaxial, $f_{c0}$, and biaxial, $f_{b0}$, yield strength of concrete in compression,
$\beta\left(\boldsymbol{\kappa}\right)=\frac{f_c\left(\boldsymbol{\kappa}\right)}{f_t\left(\boldsymbol{\kappa}\right)}\left(\alpha-1\right)-\left(1+\alpha\right)$ is a parameter that
relates tensile, $f_t\left(\boldsymbol{\kappa}\right)$, and compressive, $f_c\left(\boldsymbol{\kappa}\right)$, yield strength which are
function of a vector of damage variable, $\boldsymbol{\kappa} = \{\kappa_t, \kappa_c\}$ and $\kappa_t$
and $\kappa_c$ are the damage variables in tension and compression, respectively.

The implementation first solves the given problem in the effective stress space and then transform the effective stress to stress space using Eq. \eqref{sigma_def2}. Thus, the yield strength of the concrete under uniaxial loading is expressed as effective yield strength as follows
\begin{equation}
    f_t\left(\boldsymbol{\kappa}\right) = \left(1-D_t \left(\kappa_t\right)\right)f_{t}^{e}\left(\kappa_t\right)    \label{ft}
\end{equation}
\begin{equation}
    f_c\left(\boldsymbol{\kappa}\right) = \left(1-D_c \left(\kappa_c\right)\right)f_{c}^{e}\left(\kappa_c\right)    \label{fc}
\end{equation}
where $f_{t}^{e}$ and $f_{c}^{e}$ are the yield strength of the concrete in tension and
compression, respectively and $D_t$ and $D_c$ are the degradation damage variables in
tension and compression, respectively such that $0\leq D_t$\textless 1 and $0\leq D_c$\textless 1.
The scalar degradation damage variable is expressed in terms of $D_t$ and $D_c$ as follows
\begin{equation}
    D\left(\boldsymbol{\kappa}\right) = 1-\left(1-D_t\left(\kappa_t\right)\right)\left(1-D_c\left(\kappa_c\right)\right)    \label{D}
\end{equation}
Hence, for uniaxial tension, $D=D_t$, while for uniaxial compression, $D=D_c$.The yield strength for multi-axial loading, i.e., Eqs. \eqref{ft}-\eqref{fc}, can be rewritten as
\begin{equation}
    f_t\left(\boldsymbol{\kappa}\right) = \left(1-D\left(\boldsymbol{\kappa}\right)\right)f_{t}^{e}\left(\kappa_t\right)    \label{ft_new}
\end{equation}
\begin{equation}
    f_c\left(\boldsymbol{\kappa}\right) = \left(1-D\left(\boldsymbol{\kappa}\right)\right)f_{c}^{e}\left(\kappa_c\right)    \label{fc_new}
\end{equation}
Similarly, the first invariant of $\boldsymbol{\sigma}^e$, $I_1^e$, and second invariant of the deviatoric component of $\boldsymbol{\sigma}^e$, $J_2^e$, can be rewritten in terms of $I_1$ and $J_2$ as follows
\begin{equation}
    I_1^e = \left(1-D\left(\boldsymbol{\kappa}\right)\right)I_1    \label{I1e}
\end{equation}
\begin{equation}
    J_2^e = \left(1-D\left(\boldsymbol{\kappa}\right)\right)^2J_2    \label{J2e}
\end{equation}
Since $D$ \textless 1, the maximum principal effective stress ${\hat{\boldsymbol{\sigma}}_{max}}^e$  is expressed in the terms of ${\hat{\boldsymbol{\sigma}}_{max}}$ as follows
\begin{equation}
    {\hat{\boldsymbol{\sigma}}_{max}}^e = \left(1-D\left(\boldsymbol{\kappa}\right)\right){\hat{\boldsymbol{\sigma}}_{max}}    \label{sig_max_e}
\end{equation}
Consequently, yield function $\left(\mathfrak{F}\left(\boldsymbol{\sigma},f_t,f_c\right)\right)$ is a homogenous
function, i.e., $x \mathfrak{F}\left(\boldsymbol{\sigma},f_t,f_c\right) = \mathfrak{F}\left(x \boldsymbol{\sigma},x f_t,x f_c\right)$ Hence, using Eqs. \eqref{ft_new}-\eqref{sig_max_e}, the yield function in the effective stress space was obtained by multiplying by a factor $\left(1-D\right)$ of both sides of Eq. \eqref{yf}, as follows
\begin{equation}\label{yf_e}
\begin{gathered}
    \mathfrak{F}\left(\boldsymbol{\sigma}^e,f_t^e,f_c^e\right) = \frac{1}{1-\alpha} \\
    \left(\alpha I_1^e + \sqrt{3J_2^e} + \beta\left(\boldsymbol{\kappa}\right)<{\hat{\boldsymbol{\sigma}}_{max}}^e>\right) - f_c^e\left(\boldsymbol{\kappa}\right)
\end{gathered}
\end{equation}

## Plastic Potential

It was found that for concrete, the Drucker-Prager flow rule describes the experimentally observed volumetric expansion of the material as opposed to the Von-Mises flow rule, which results in no volumetric expansion ([!cite](krabbenhoft2002basic)). Since all the equations are solved in the effective stress space, the plastic flow potential is also defined in the effective stress space ([!cite](lee1996theory)) as follows
\begin{equation}
    \dot{\boldsymbol{\varepsilon}^p} = \dot{\gamma} \nabla_{\boldsymbol{\sigma}^e}\Phi\left(\boldsymbol{\sigma}^e\right)
\end{equation}
\begin{equation} \label{flowRule}
    \Phi\left(\boldsymbol{\sigma}^e\right)=\alpha_p I_1^e+\|{s^e}\|
\end{equation}
where $\Phi$ is the plastic potential function, $s^e$ is the deviatoric component of the
$\boldsymbol{\sigma}^e$, and $\|\cdot\|$ is $L_2$ norm of $\alpha_p$ is a parameter that controls the
dilatancy of concrete, and $\dot{\gamma}$ is the plastic consistency parameter.

## Strength Function

Since the concrete shows strain-softening in tension and strain hardening and softening in compression, the concrete strength is expressed as a combination of two exponential functions as follows
\begin{equation}
    f_N = f_{N0} \left(\left(1+a_N\right) e^{-b_N \varepsilon^p}- a_N e^{-2b_N \varepsilon^p}\right) \label{fN}
\end{equation}
where $f_{N0}$ is the initial yield stress of the material, $N = t$, for the uniaxial
tension, $N = c$, for uniaxial compression, $a_N$ and $b_N$, are the material constants
that describe the softening and hardening behavior of the concrete. Similarly, the
degradation of the elastic modulus is also expressed as another exponential function as
follows
\begin{equation}
    D_N = 1 - e^{-d_N \varepsilon^p} \label{DN}
\end{equation}
where $d_N$ is a constant that determine the rate of degradation of $\boldsymbol{\mathfrak{E}}$ with the
increase in plastic strain. The strength of the material in the effective stress space was
obtained using Eqs. \eqref{ft_new}-\eqref{fc_new}, and \eqref{fN}-\eqref{DN}, as follows
\begin{equation}\label{fNe}
    f_N^e = f_{N0} \left(\left(1+a_N\right) \left(e^{-b_N \varepsilon^p}\right)^{1-\frac{d_N}{b_N}}-
    a_N \left(e^{-b_N \varepsilon^p}\right)^{2-\frac{d_N}{b_N}}\right)
\end{equation}
The damage variable, $\kappa_N$ is defined as
\begin{equation}
    \kappa_N = \frac{1}{g_N}\int_0^{\varepsilon^p} {f_N\left(\varepsilon^p\right)d\varepsilon^p} \label{kN_def}
\end{equation}
where $g_N$  $\left(=\int_0^\infty {f_N\left(\varepsilon^p\right)d\varepsilon^p}=\frac {f_{N0}}{b_N}\left(1+\frac{a_N}{2}\right)\right)$
is the fracture energy density during the process of cracking, which is derived from the
fracture energy, $G_N$, which is a material property. The relation between $G_N$ and $g_N$
is expressed as follows
\begin{equation}
    g_N = \frac{G_N}{l_N} \label{GN_def}
\end{equation}
where $l_N$ is characteristic length or the size of the deformation localization zone.
Thus, the plastic strain can be presented in terms of damage variable as follows
\begin{equation}
    \varepsilon^p = \frac{1}{b_N} \log{\frac{\sqrt{\Phi_N}}{a_N}} \label{eps_p}
\end{equation}
where $\Phi_N = 1 + a_N \left(2+a_N \right)\kappa_N$. Using Eqs. \eqref{fN} and \eqref{eps_p}, the
strength of the concrete can be expressed in terms of the damage variable as follows
\begin{equation}
    f_N = f_{N0} \frac{1+a_N-\sqrt{\Phi_N\left(\kappa_N\right)}}{a_N}\sqrt{\Phi_N\left(\kappa_N\right)} \label{fN_new}
\end{equation}
Thus, the strength of the material and degradation damage variable in the effective stress space can be written as

\begin{equation}
    f_N^e = f_{N0} \left(\frac{1+a_N-\sqrt{\Phi_N\left(\kappa_N\right)}}{a_N}\right)^{1-\frac{d_N}{b_N}} \sqrt{\Phi_N\left(\kappa_N\right)} \label{fNe_new}
\end{equation}
\begin{equation}
    D_N = 1- \left(\frac{1+a_N-\sqrt{\Phi_N\left(\kappa_N\right)}}{a_N}\right)^{\frac{d_N}{b_N}}  \label{DN_new}
\end{equation}
where $a_N$, $b_N$,and $d_N$ are the modeling parameters, which are evaluated from the
material properties. Since the maximum compressive strength of concrete, $f_{cm}$, was used
as a material property, $f_{cm}$ was obtained in terms of $a_c$ by finding maximum value of
compressive strength in Eq. \eqref{fNe} as follows
\begin{equation}
    f_{cm} = \frac{f_{c0}\left(1+a_c\right)^2}{4a_c}  \label{fcm}
\end{equation}
Thus, $a_c$ can be expressed as follows
\begin{equation}
    a_c = 2\frac{f_{cm}}{f_{c0}}-1+2\sqrt{\left(\frac{f_{cm}}{f_{c0}}\right)^2-\frac{f_{cm}}{f_{c0}}} \label{ac}
\end{equation}
Similarly, if $G_c$ and $l_c$ are known then $b_c$ can be expressed in term of known quantities as follows
\begin{equation}
    b_c = \frac{f_{c0}}{\frac{G_c}{l_c}}\left(1+\frac{a_c}{2}\right) \label{bc}
\end{equation}
A relationship between $a_t$ and $b_t$ is written as follows
\begin{equation}
    b_t = \frac{f_{t0}}{\frac{G_t}{l_t}}\left(1+\frac{a_t}{2}\right) \label{bt}
\end{equation}
[!cite](lubliner1989plastic) suggested that if the slope of $\sigma$ versus $\varepsilon^p$ curve is
known at $\varepsilon^p=0$, then another relationship between $a_t$ and $b_t$ will be obtained
as follows
\begin{equation}
    \left(\frac{d\sigma}{d\varepsilon^p}\right)_{\varepsilon^p=0} = f_{t0}b_t\left(a_t-1\right) \label{slope}
\end{equation}
Thus, $a_t$ was obtained using Eqs. \eqref{bt}-\eqref{slope} as follows
\begin{equation}
     a_t = \sqrt{\frac{9}{4}+\frac{2\frac{G_t}{l_t} \left(\frac{d\sigma}{d\varepsilon^p}\right)_{\varepsilon^p=0}}{f_{t0}^2}}\label{at}
\end{equation}
The minimum slope of the $\sigma$ versus $\varepsilon^p$ curve is
$\left(\left(\frac{d\sigma}{d\varepsilon^p}\right)_{\varepsilon^p=0}\right)_{min}=
-\frac{9}{8}\frac{f_{t0}^2}{\frac{G_t}{l_t}}$, which is a function of the characteristic length in tension.
Therefore, a mesh independent slope parameter $\omega\in\left(0,1\right)$, is defined such that
\begin{equation}
    \left(\frac{d\boldsymbol{\sigma}}{d\varepsilon^p}\right)_{\varepsilon^p=0} = \omega \left(\left(\frac{d\sigma}{d\varepsilon^p}\right)_{\varepsilon^p=0}\right)_{min} \label{slope_new}
\end{equation}
Using Eqs. \eqref{at}-\eqref{slope_new}, $a_t$ is rewritten as follows
\begin{equation}
    a_t = \frac{3}{2}\sqrt{1-\omega}-\frac{1}{2}\label{at_new}
\end{equation}
The ratio of $\frac{d_c}{b_c}$ was obtained by specifying degradation values for uniaxial
compression case from experiments. If the degradation in the elastic modulus is known,
denoted as $\widetilde{D}_c$, when the concrete is unloaded from $\sigma =f_{cm}$, then $\frac{d_c}{b_c}$ will be obtained using the following relation
\begin{equation}
    \widetilde{D}_c = 1 - \left(\frac{1+a_c}{2a_c}\right)^{\frac{d_c}{b_c}} \label{Dc_fcm}
\end{equation}
\begin{equation}
    \frac{d_c}{b_c} = \frac{\log\left(1-\widetilde{D}_c\right)}{\log\left(\frac{1+a_c}{2a_c}\right)} \label{dcbc_fcm}
\end{equation}
Similarly, if degradation in the elastic modulus is known, denoted as $\widetilde{D}_t$, when the material is unloaded from $\sigma=\frac {f_{t0}}{2}$, on softening branch, then $\frac{d_t}{b_t}$ will be obtained using the following relation
\begin{equation}
    \widetilde{D}_t = 1 - \left(\frac{1+a_t-\sqrt{1+a_t^2}}{2a_t}\right)^{\frac{d_t}{b_t}} \label{Dt_ft0_2}
\end{equation}
\begin{equation}
    \frac{d_t}{b_t} = \frac{\log\left(1-\widetilde{D}_t\right)}{\log\left(\frac{1+a_t-\sqrt{1+a_t^2}}{2a_t}\right)} \label{Dt_ft0}
\end{equation}
Thus, material modeling parameters $a_N$,$b_N$, and $d_N$ were obtained, which were used in
defining the strength of concrete in both tension and compression as given in Eq.
\eqref{fNe_new}. These parameters are also used to define the degradation damage variable in
both tension and compression as indicated in Eq. \eqref{DN_new}.

## Hardening Potential

The vector of two damage variables, $\boldsymbol{\kappa}=\{\kappa_t, \kappa_c\}$, was used in the implementation as the state variable to store the state of damage in tension and compression, separately. The evolution of these damage variables is defined in terms of the hardening potential, $H$, as
\begin{equation}
     \dot{\boldsymbol{\kappa}} = \dot{\gamma} H\left(\boldsymbol{\sigma}^e, \boldsymbol{\kappa}\right)\label{kappa}
\end{equation}
The evolution of the damage variable is expressed in terms of the evolution of $\boldsymbol{\varepsilon}^p$ as follows
\begin{equation}
    \dot{\boldsymbol{\kappa}} = \frac{1}{g_N}f_N^e\left(\kappa_N\right)\dot{\boldsymbol{\varepsilon}^p} \label{kappa_ep}
\end{equation}
where $g_N$ is dissipated energy density during the process of cracking. The scalar $\dot{\boldsymbol{\varepsilon}^p}$, is extended to multi-dimensional case as follows
\begin{equation}
    \dot{\boldsymbol{\varepsilon}^p} = \delta_{tN} r\left(\hat{\boldsymbol{\sigma}^e}\right)\dot{\varepsilon}^{p}_{max} + \delta_{cN} \left(1-r\left(\hat{\boldsymbol{\sigma}^e}\right)\right)\dot{\varepsilon}^{p}_{min} \label{ep_dot}
\end{equation}
where $\delta_{ij}$ is the Dirac delta function and $\hat{\boldsymbol{\sigma}^e}$ are eigenvalues of the $\boldsymbol{\sigma}^e$,
\begin{equation}\label{r_sige}
    r\left(\hat{\boldsymbol{\sigma}^e}\right) =
    \begin{cases}
        0,& \text{if } \boldsymbol{\sigma}^e = \boldsymbol{0}\\
        \frac{\sum_{i=1}^3<\sigma^e_i>}{\sum_{i=1}^3|\sigma^e_i|}, & \text{otherwise}
    \end{cases}
\end{equation}
$\dot{\varepsilon}^{p}_{max}$ and $\dot{\varepsilon}^{p}_{min}$
are the maximum and minimum principal plastic strain, respectively. From Eqs. \eqref{kappa_ep} - \eqref{r_sige}, the evolution of $\boldsymbol{\kappa}$ was obtained as
\begin{equation}
    \dot{\boldsymbol{\kappa}} = \boldsymbol{h}\left(\hat{\boldsymbol{\sigma}^e}\right):\dot{\boldsymbol{\varepsilon}}^{\hat{p}} \label{kappa_h_ep}
\end{equation}
where
\begin{equation}\label{h}
    \boldsymbol{h}\left(\hat{\boldsymbol{\sigma}^e}\right)=
    \begin{bmatrix}
    \frac{r\left(\hat{\boldsymbol{\sigma}^e}\right)}{g_t}f_t^e\left(\kappa_t\right)&0&0\\
    0&1&0\\
    0&0&\frac{1-r\left(\hat{\boldsymbol{\sigma}^e}\right)}{g_c}f_c^e\left(\kappa_c\right)\\
    \end{bmatrix}
\end{equation}
and ‘:’ represents products of two matrices. Hence, $H\left(\boldsymbol{\sigma}^e,\boldsymbol{\kappa}\right)$ in Eq. \eqref{kappa} was obtained as follows
\begin{equation}
     H\left(\boldsymbol{\sigma}^e, \boldsymbol{\kappa}\right) = \boldsymbol{h}\cdot \nabla_{\hat{\boldsymbol{\sigma}^e}}\Phi\left(\hat{\boldsymbol{\sigma}^e}\right) \label{H_def}
\end{equation}
where ‘$\cdot$’ represents the dot product of a matrix and a vector, and $\nabla_{\hat{\boldsymbol{\sigma}^e}}$ is the gradient with respect to principal effective stress components, $\hat{\boldsymbol{\sigma}^e}$. Thus, the hardening potential that governs the evolution of damage variables is expressed in terms of effective stress space.

## Return Mapping Algorithm

Return mapping algorithm is summarized in a flowchart [flowChart]. For given strain increment and previous state of stress, trial effective stress is obtained by elastic increment in the elastic predictor step according to
\begin{equation}
	\boldsymbol{\sigma}_{n+1}^{e^{tr}} = \boldsymbol{\mathfrak{E}}_0:\left(\varepsilon_{n+1}-\varepsilon_n^p\right) \label{predictor_step}
\end{equation}
If the state of trial effective stress lies inside the admissible domain, i.e, $\mathfrak{F}\left(\boldsymbol{\sigma}^e,f_t^e,f_c^e\right) < 0$, the trial effective stress is considered as an admissible effective stress and old damage variables are supplied for the next step.
If the state of trial effective stress is outside admissible domain, i.e, $\mathfrak{F}\left(\boldsymbol{\sigma}^e,f_t^e,f_c^e\right) > 0$, the effective stress is obtained using the plastic corrector step

!media media/Return_mapping_flow_chart.png
      id=flowChart
      style=width:30%;
      caption=Flow chart for return mapping algorithm.

\begin{equation}
\boldsymbol{\sigma}_{n+1}^e = \boldsymbol{\sigma}_{n+1}^{e^{tr}}-\boldsymbol{\mathfrak{E}}_0:\varepsilon_n^p \label{plasticCorrector}
\end{equation}
Once admissible effective stress is obtained, degradation corrector step is utilized to account for the stiffness degradation on the state of stress according to
\begin{equation}
\boldsymbol{\sigma}_{n+1} = \left(1-D_{n}\right)\boldsymbol{\sigma}_{n+1}^{e} \label{degradation_corrector}
\end{equation}



During the plastic corrector step, the returned effective stress should satisfy the Kuhn-Tucker conditions on the  $\mathfrak{F}\left(\boldsymbol{\sigma}^e,f_t^e,f_c^e\right)$ and $\dot{\gamma}$, according to
\begin{equation}
    \begin{split} \label{khunTuckerConditions}
        \dot{\gamma} > 0  \\
        \dot{\gamma}\mathfrak{F}\left(\boldsymbol{\sigma}^e,f_t^e,f_c^e\right) = 0 \\
        \mathfrak{F}\left(\boldsymbol{\sigma}^e,f_t^e,f_c^e\right) = 0
    \end{split}
\end{equation}
As per flow rule in Eq. \eqref{flowRule}, the plastic corrector step, i.e., Eq. \eqref{plasticCorrector} can be rewritten as
\begin{equation}
\boldsymbol{\sigma^e}_{n+1} = \boldsymbol{\sigma}_{n+1}^{e^{tr}}-\dot{\gamma}\left(2G\frac{\boldsymbol{s}_{n+1}^e}{\|\boldsymbol{s}_{n+1}^e\|} + 3K\alpha_p\boldsymbol{I}\right) \label{returnMap1}
\end{equation}
where $G$ is shear modulus and $K$ is bulk modulus. After separating the volumetric and deviatoric components from Eq. \eqref{returnMap1} following relations can be obtained
\begin{equation}
    I_{1|n+1} = I_{1|n+1}^{e^{tr}} - 9K\alpha \alpha_p \dot{\gamma} \label{stressRelation1}
\end{equation}
\begin{equation}\label{stressRelation2}
    \begin{gathered}
        \frac{\boldsymbol{s}_{n+1}^e}{\|\boldsymbol{s}_{n+1}^e\|} = \frac{\boldsymbol{s}_{{n+1}}^{e^{tr}}}{\|\boldsymbol{s}_{n+1}^{e^{tr}}\|} \\
	{\|\boldsymbol{s}^{e}_{n+1}\|} = {\|\boldsymbol{s}_{n+1}^{e^{tr}}\|} - 2G\dot{\gamma}
    \end{gathered}
\end{equation}
Using Eqs. \eqref{stressRelation1} and \eqref{stressRelation2}, Eq. \eqref{returnMap1} can be written as
\begin{equation}
	\boldsymbol{\sigma}_{n+1}^e = \boldsymbol{\sigma}_{n+1}^{e^{tr}}-\dot{\gamma}\left(2G\frac{\boldsymbol{s}^{e^{tr}}_{n+1}}{\|\boldsymbol{s}_{{n+1}}^{e^{tr}} \|}+ 3K\alpha_p\boldsymbol{I}\right) \label{returnMap2}
\end{equation}
In case of plastic deformation, the returned state of stress should lie on the yield surface as per Kuhn-Tucker conditions (Eq. \eqref{khunTuckerConditions}, therefore $\mathfrak{F}\left(\boldsymbol{\sigma}_{n+1}^e,f_t^e,f_c^e\right) = 0$, i.e.,
\begin{equation} \label{yfnext}
    \begin{gathered}
    	\alpha I_{1|n+1}^e + \sqrt{3J_{2|n+1}^e} + \beta\left(\boldsymbol{\kappa}\right)<\hat{\boldsymbol{\sigma}}^e_{n+1|max}> \\ - \left(1-\alpha\right)f_c^e\left(\boldsymbol{\kappa}\right) = 0    
    \end{gathered}
\end{equation}
Using Eq. \eqref{stressRelation1}, \eqref{stressRelation2}, and \eqref{returnMap2}, Eq. \eqref{yfnext} can be written as
\begin{equation} \label{yfzero}
    \begin{gathered}
    	\alpha\left(I_{1|n+1}^{e^{tr}} - 9K\alpha \alpha_p \dot{\gamma}\right) +
    	\left(\sqrt{\frac{3}{2}}\|\boldsymbol{s}_{{n+1}}^{e^{tr}}\| - \sqrt{6}G\dot{\gamma}\right)\\+
    	\beta\left(\boldsymbol{\kappa}\right)<\hat{\boldsymbol{\sigma}}^e_{n+1|max}> - \left(1-\alpha\right)f_c^e\left(\boldsymbol{\kappa}\right) = 0
    \end{gathered}
\end{equation}
Thus, the plastic multiplier can be by solving Eq. \eqref{yfzero} as
\begin{equation}\label{gammaDef}
    \dot{\gamma} =
    \begin{cases}
        \frac{\alpha I_{1|n+1}^{e^{tr}}+\sqrt{\frac{3}{2}}\|\boldsymbol{s}_{{n+1}}^{e^{tr}}\|-\left(1-\alpha\right)f_c^e\left(\boldsymbol{\kappa}\right)}
        {9K \alpha_p + \sqrt{6}G}, & \text{if $\sigma_{m|n+1}^e < 0$}\\
        \frac{\alpha I_{1|n+1}^{e^{tr}}+\sqrt{\frac{3}{2}}\|\boldsymbol{s}_{{n+1}}^{e^{tr}}\|+\beta\left(\boldsymbol{\kappa}\right) \sigma_{m|n+1}^{e^{tr}}-\left(1-\alpha\right)f_c^e\left(\boldsymbol{\kappa}\right)}
        {9K \alpha_p + \sqrt{6}G + \beta\left(\boldsymbol{\kappa}\right)\left(2G\frac{s^{e^{tr}}_{m|n+1}}{\|\boldsymbol{s}_{{n+1}}^{e^{tr}} \|}+ 3K\alpha_p\right)}, & \text{otherwise}.
  \end{cases}
\end{equation}
where $\sigma_{m|n+1}^e$, $\sigma_{m|n+1}^{e^{tr}}$, and $s^{e^{tr}}_{m|n+1}$ are the $m^{th}$ component of the $\hat{\boldsymbol{\sigma}}_{n+1}^e$, $\boldsymbol{\sigma}_{n+1}^{e^{tr}}$, and $\boldsymbol{s}^{e^{tr}}_{n+1}$, respectively, which corresponds to maximum principal effective stress in $\left(n+1\right)^{th}$ step. Eq. \eqref{gammaDef} is solved iteratively.


!syntax parameters /Materials/DamagePlasticityStressUpdate

!syntax inputs /Materials/DamagePlasticityStressUpdate

!syntax children /Materials/DamagePlasticityStressUpdate

!bibtex bibliography
