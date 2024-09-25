import clsx from 'clsx';
import Heading from '@theme/Heading';
import styles from './styles.module.css';

const FeatureList = [
  {
    title: 'Easy to Use',
    Svg: require('@site/static/img/chart_icon.svg').default,
    description: (
      <>
        Create and run motion profiles in seconds! With an intuitive user interface, you can quickly set up your desired motion parameters.
      </>
    ),
  },
  {
    title: 'Focus on What Matters',
    Svg: require('@site/static/img/focus_icon.svg').default,
    description: (
      <>
        Docusaurus lets you focus on your system and planning your motion profile. Spend less time on documentation and more time developing your project. 
      </>
    ),
  },
  {
    title: 'Developed in C++',
    Svg: require('@site/static/img/cpp_icon.svg').default,
    description: (
      <>
        Developed in C++ for maximum efficiency. Using Dear ImGui for graphical interfaces ensures a smooth and responsive user experience. Leverage the power of C++ to handle complex calculations quickly, making your simulations run faster and more effectively.
      </>
    ),
  },
];

function Feature({Svg, title, description}) {
  return (
    <div className={clsx('col col--4')}>
      <div className="text--center">
        <Svg className={styles.featureSvg} role="img" />
      </div>
      <div className="text--center padding-horiz--md">
        <Heading as="h3">{title}</Heading>
        <p>{description}</p>
      </div>
    </div>
  );
}

export default function HomepageFeatures() {
  return (
    <section className={styles.features}>
      <div className="container">
        <div className="row">
          {FeatureList.map((props, idx) => (
            <Feature key={idx} {...props} />
          ))}
        </div>
      </div>
    </section>
  );
}
